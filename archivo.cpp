#include "archivo.h"
#include "definiciones.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Implementacion concreta del manejador de versiones */

/* ------------------ Estructuras internas ------------------ */

typedef enum { TYPE_IL, TYPE_BL } ChangeType;

typedef struct change {
    ChangeType type;
    unsigned int line; /* 1-based line number at the time of the change */
    char *text; /* only for IL */
    struct change *next;
} Change;

typedef struct version {
    struct version *parent;
    struct version *firstChild;
    struct version *nextSibling;
    Change *changes; /* cambios realizados directamente en esta version respecto a su padre, en orden */
} Version;

struct nodo_archivo {
    char *nombre;
    Version *firstVersion; /* lista de versiones de primer nivel (hermanos) */
};

/* ------------------ Helpers para manejo de memoria ------------------ */

static char *strdup_safe(const char *s) {
    if (!s) return NULL;
    char *r = (char*) malloc(strlen(s) + 1);
    if (!r) return NULL;
    strcpy(r, s);
    return r;
}

static void free_change_list(Change *c) {
    while (c) {
        Change *tmp = c->next;
        if (c->text) free(c->text);
        free(c);
        c = tmp;
    }
}

static void free_version_subtree(Version *v) {
    if (!v) return;
    /* liberar hijos recursivamente */
    Version *child = v->firstChild;
    while (child) {
        Version *nextChild = child->nextSibling;
        free_version_subtree(child);
        child = nextChild;
    }
    /* liberar cambios de esta versión */
    free_change_list(v->changes);
    /* finalmente liberar nodo */
    free(v);
}

/* ------------------ Parseo de etiqueta "1.2.3" -> vector de enteros ------------------ */

static bool parse_version_string(const char *s, unsigned int **out, unsigned int *len) {
    if (!s || !*s) return false;
    unsigned int capacity = 8;
    unsigned int count = 0;
    unsigned int *arr = (unsigned int*) malloc(sizeof(unsigned int) * capacity);
    const char *p = s;
    while (*p) {
        if (!isdigit((unsigned char)*p)) { free(arr); return false; }
        unsigned int val = 0;
        while (isdigit((unsigned char)*p)) {
            val = val * 10 + (*p - '0');
            p++;
        }
        if (val == 0) { free(arr); return false; } /* versión no puede tener 0 */
        if (count >= capacity) {
            capacity *= 2;
            arr = (unsigned int*) realloc(arr, sizeof(unsigned int) * capacity);
        }
        arr[count++] = val;
        if (*p == '.') {
            p++;
            if (!*p) { free(arr); return false; } /* no termina con punto */
        } else if (*p == '\0') {
            break;
        } else { free(arr); return false; }
    }
    *out = arr;
    *len = count;
    return true;
}

/* ------------------ Navegación árbol: encontrar nodo dado arreglo de índices ------------------ */
/* Nota: la posición en la lista de hijos define la numeración (1-based). */

static Version *get_node_by_sequence(Version *firstLevelList, unsigned int *seq, unsigned int seqlen) {
    if (seqlen == 0) return NULL;
    Version *currentList = firstLevelList;
    Version *node = NULL;
    for (unsigned int i = 0; i < seqlen; ++i) {
        unsigned int desiredPos = seq[i]; /* 1-based */
        unsigned int pos = 1;
        Version *v = currentList;
        while (v && pos < desiredPos) {
            v = v->nextSibling;
            pos++;
        }
        if (!v || pos != desiredPos) {
            return NULL; /* no existe */
        }
        node = v;
        /* siguiente nivel: los hijos del nodo */
        currentList = node->firstChild;
    }
    return node;
}

/* Devuelve el numero de hijos (conteo) de una lista de hermanos */
static unsigned int count_children(Version *firstChild) {
    unsigned int c = 0;
    Version *v = firstChild;
    while (v) { c++; v = v->nextSibling; }
    return c;
}

/* Inserta un nuevo hijo en position pos (1-based). Si pos == count+1 => append.
   Devuelve puntero al nuevo nodo insertado.
*/
static Version *insert_child_at(Version **pFirstChild, unsigned int pos) {
    unsigned int cnt = count_children(*pFirstChild);
    if (pos < 1 || pos > cnt + 1) return NULL;
    Version *newv = (Version*) malloc(sizeof(Version));
    if (!newv) return NULL;
    newv->parent = NULL; newv->firstChild = NULL; newv->nextSibling = NULL; newv->changes = NULL;
    if (pos == 1) {
        /* insertar al comienzo */
        newv->nextSibling = *pFirstChild;
        *pFirstChild = newv;
        return newv;
    }
    /* buscar el (pos-1)-ésimo */
    Version *prev = *pFirstChild;
    for (unsigned int i = 1; i < pos - 1; ++i) prev = prev->nextSibling;
    newv->nextSibling = prev->nextSibling;
    prev->nextSibling = newv;
    return newv;
}

/* Remueve de la lista de hijos la posición pos (1-based) y devuelve el nodo removido (no libre) */
static Version *remove_child_at(Version **pFirstChild, unsigned int pos) {
    unsigned int cnt = count_children(*pFirstChild);
    if (pos < 1 || pos > cnt) return NULL;
    Version *removed = NULL;
    if (pos == 1) {
        removed = *pFirstChild;
        *pFirstChild = removed->nextSibling;
        removed->nextSibling = NULL;
        return removed;
    }
    Version *prev = *pFirstChild;
    for (unsigned int i = 1; i < pos - 1; ++i) prev = prev->nextSibling;
    removed = prev->nextSibling;
    prev->nextSibling = removed->nextSibling;
    removed->nextSibling = NULL;
    return removed;
}

/* Encuentra la posición (1-based) de un nodo entre sus hermanos (returns 0 si no encontrado) */
static unsigned int position_in_siblings(Version *first, Version *target) {
    unsigned int pos = 1;
    Version *v = first;
    while (v) {
        if (v == target) return pos;
        v = v->nextSibling;
        pos++;
    }
    return 0;
}

/* ------------------ Operaciones sobre texto: obtener texto completo de una versión ------------------ */

/* Representación temporal del texto en memoria: vector de char* dinámico. */
typedef struct text_array {
    char **lines;
    unsigned int n;
    unsigned int cap;
} text_array;

static void text_array_init(text_array *t) { t->cap = 8; t->n = 0; t->lines = (char**) malloc(sizeof(char*) * t->cap); }
static void text_array_free(text_array *t) {
    for (unsigned int i = 0; i < t->n; ++i) free(t->lines[i]);
    free(t->lines);
    t->lines = NULL; t->n = 0; t->cap = 0;
}
static void text_array_insert(text_array *t, unsigned int pos /*1-based*/, const char *s) {
    if (pos < 1) pos = 1;
    if (t->n + 1 > t->cap) {
        t->cap *= 2;
        t->lines = (char**) realloc(t->lines, sizeof(char*) * t->cap);
    }
    unsigned int idx = pos - 1;
    if (idx > t->n) idx = t->n;
    for (unsigned int i = t->n; i > idx; --i) t->lines[i] = t->lines[i-1];
    t->lines[idx] = strdup_safe(s);
    t->n++;
}
static void text_array_delete(text_array *t, unsigned int pos /*1-based*/) {
    if (pos < 1 || pos > t->n) return;
    unsigned int idx = pos - 1;
    free(t->lines[idx]);
    for (unsigned int i = idx; i + 1 < t->n; ++i) t->lines[i] = t->lines[i+1];
    t->n--;
}

/* Construye el texto completo de la versión v aplicando cambios de ancestros */
static void build_text_for_version(Version *v, text_array *out) {
    text_array_init(out);
    /* Recolectar la cadena de ancestros desde raíz (primer nivel) hacia abajo */
    /* Para eso, armar un arreglo de pointers desde el primer-level ancestor hasta v */
    unsigned int depth = 0;
    Version *ptr = v;
    while (ptr) { depth++; ptr = ptr->parent; }
    Version **stack = (Version**) malloc(sizeof(Version*) * depth);
    ptr = v;
    for (unsigned int i = 0; i < depth; ++i) {
        stack[depth - 1 - i] = ptr; /* invertimos para tener raíz primero */
        ptr = ptr->parent;
    }
    /* Aplicar cambios en orden desde el ancestro más alto hasta v */
    for (unsigned int i = 0; i < depth; ++i) {
        Change *c = stack[i]->changes;
        while (c) {
            if (c->type == TYPE_IL) {
                /* insertar */
                text_array_insert(out, c->line, c->text ? c->text : "");
            } else {
                /* borrar */
                text_array_delete(out, c->line);
            }
            c = c->next;
        }
    }
    free(stack);
}

/* ------------------ Utilities para mostrar etiquetas (build "1.2.3" a partir de nodo) ------------------ */

/* Construye la etiqueta de la versión separada por '.' y la coloca en buffer (buffer deberá ser liberado por caller) */
static char *build_label_for_node(Version *v) {
    /* calcular profundidad y recoger los nodos */
    unsigned int depth = 0;
    Version *p = v;
    while (p) { depth++; p = p->parent; }
    Version **arr = (Version**) malloc(sizeof(Version*) * depth);
    p = v;
    for (unsigned int i = 0; i < depth; ++i) {
        arr[depth - 1 - i] = p; /* invertido: raíz primero */
        p = p->parent;
    }
    /* para cada nivel calcular la posición (1-based) dentro de sus hermanos */
    /* construir string dinámicamente */
    char tmp[256];
    tmp[0] = '\0';
    size_t needed = 1;
    char *res = (char*) malloc(1);
    res[0] = '\0';
    for (unsigned int i = 0; i < depth; ++i) {
        Version *node = arr[i];
        Version *firstSib;
        if (node->parent == NULL) firstSib = node->parent == NULL ? arr[0] : NULL; /* placeholder not used */
        /* if parent is NULL, first sibling list is the file's firstVersion; but we don't have access here.
           We'll compute position by scanning siblings starting from the parent's firstChild.
           To get parent's firstChild, use node->parent->firstChild or, if parent==NULL, we need caller to provide first-level list.
           Simpler: if parent == NULL, the siblings are those nodes at top-level; but we cannot access the file root here.
           To solve this, we will compute position by scanning backwards using node's parent's firstChild when parent exists.
           For root-level nodes, we'll find the first sibling by walking left: go to node and walk backwards by looking up from parent is NULL impossible.
           Therefore, for root level we must scan siblings by walking to previous nodes which we cannot do easily. As a workaround,
           we'll compute position by counting from node->parent? For root-level, we can count by iterating siblings starting at node and moving backwards is not possible.
           To avoid complexity, we will determine position by counting from the beginning: go to parent's firstChild when parent exists; for parent==NULL we will find first by walking left from node using a helper not currently available.
        */
    }
    free(arr);
    /* NOTE: the above attempt to compute label in this helper has complexity because we need the pointer to the first sibling.
       We'll avoid this helper complexity by passing the firstChild pointer from caller where needed (we do so below).
    */
    return res;
}

/* Instead of the above broken helper, implement a recursive printer that knows the starting first-level list. */

/* Imprime recursivamente versiones, construyendo el prefijo de numeros en un arreglo int */
static void print_versions_recursive(Version *firstLevelList, FILE *out, unsigned int *prefix, unsigned int prefix_len, Version *current) {
    /* current is the node to process in the siblings list (iterate externally) */
    Version *v = current;
    unsigned int pos = 1;
    while (v) {
        prefix[prefix_len] = pos;
        /* imprimir etiqueta con tabs segun prefix_len */
        for (unsigned int t = 0; t < prefix_len; ++t) fputc('\t', out);
        /* imprimir label */
        for (unsigned int k = 0; k <= prefix_len; ++k) {
            if (k) fputc('.', out);
            fprintf(out, "%u", prefix[k]);
        }
        fprintf(out, " \n");
        /* recursión en hijos */
        if (v->firstChild) {
            print_versions_recursive(firstLevelList, out, prefix, prefix_len + 1, v->firstChild);
        }
        v = v->nextSibling;
        pos++;
    }
}

/* Wrapper para MostrarVersiones: crea buffer de prefijo grande suficiente e imprime desde firstVersion */
static TipoRet mostrar_versiones_impl(struct nodo_archivo *a) {
    printf("%s\n\n", a->nombre ? a->nombre : "");
    if (!a->firstVersion) {
        printf("No hay versiones creadas\n");
        return OK;
    }
    /* calcular max profundidad posible (conservador) */
    unsigned int maxDepth = 64;
    unsigned int *prefix = (unsigned int*) calloc(maxDepth, sizeof(unsigned int));
    /* Primer nivel: iterar sobre firstVersion list y usar print_versions_recursive */
    print_versions_recursive(a->firstVersion, stdout, prefix, 0, a->firstVersion);
    free(prefix);
    return OK;
}

/* ------------------ Operaciones públicas ------------------ */

Archivo CrearArchivo(char * nombre) {
    struct nodo_archivo *a = (struct nodo_archivo*) malloc(sizeof(struct nodo_archivo));
    if (!a) return NULL;
    a->nombre = strdup_safe(nombre ? nombre : "");
    a->firstVersion = NULL;
    return (Archivo) a;
}

TipoRet BorrarArchivo(Archivo &a) {
    if (a == NULL) return OK;
    struct nodo_archivo *ap = (struct nodo_archivo*) a;
    /* liberar todas las versiones recursivamente */
    Version *v = ap->firstVersion;
    while (v) {
        Version *nextv = v->nextSibling;
        free_version_subtree(v);
        v = nextv;
    }
    if (ap->nombre) free(ap->nombre);
    free(ap);
    a = NULL;
    return OK;
}

/* CrearVersion:
   - parsea la etiqueta en secuencia
   - si se pide primer nivel (seqlen==1): parent = NULL; comprobar que pos <= count+1
   - si se pide seqlen>1: localizar nodo parent (prefix sin el ultimo) y comprobar que parent exista
   - comprobar que pos (ultimo número) <= siblings_count + 1 (no huecos)
   - insertar new child en la posición solicitada
*/
TipoRet CrearVersion(Archivo &a, char * version, char * error) {
    if (!a) { if (error) strcpy(error, "Archivo no existe"); return ERROR; }
    unsigned int *seq = NULL, seqlen = 0;
    if (!parse_version_string(version, &seq, &seqlen)) {
        if (error) strcpy(error, "Version invalida");
        return ERROR;
    }
    struct nodo_archivo *ap = (struct nodo_archivo*) a;
    Version *parent = NULL;
    if (seqlen == 1) {
        /* primer nivel */
        unsigned int pos = seq[0];
        unsigned int cnt = count_children(ap->firstVersion);
        if (pos > cnt + 1) { free(seq); if (error) strcpy(error, "Huecos entre versiones hermanas"); return ERROR; }
        Version *newv = insert_child_at(&ap->firstVersion, pos);
        if (!newv) { free(seq); if (error) strcpy(error, "No se pudo crear version"); return ERROR; }
        newv->parent = NULL;
        free(seq);
        return OK;
    } else {
        /* localizar padre: seq[0..seqlen-2] */
        unsigned int parentLen = seqlen - 1;
        /* buscar parent */
        Version *node = get_node_by_sequence(ap->firstVersion, seq, parentLen);
        if (!node) { free(seq); if (error) strcpy(error, "Version padre inexistente"); return ERROR; }
        parent = node;
        unsigned int pos = seq[seqlen - 1];
        unsigned int cnt = count_children(parent->firstChild);
        if (pos > cnt + 1) { free(seq); if (error) strcpy(error, "Huecos entre versiones hermanas"); return ERROR; }
        Version *newv = insert_child_at(&parent->firstChild, pos);
        if (!newv) { free(seq); if (error) strcpy(error, "No se pudo crear version"); return ERROR; }
        newv->parent = parent;
        free(seq);
        return OK;
    }
}

/* BorrarVersion: localizar nodo. si no existe -> ERROR.
   Si existe, removerlo de la lista de hijos del padre y liberar subtree.
*/
TipoRet BorrarVersion(Archivo &a, char * version) {
    if (!a) return ERROR;
    unsigned int *seq = NULL, seqlen = 0;
    if (!parse_version_string(version, &seq, &seqlen)) { free(seq); return ERROR; }
    struct nodo_archivo *ap = (struct nodo_archivo*) a;
    if (seqlen == 1) {
        /* remover de firstVersion */
        unsigned int pos = seq[0];
        unsigned int cnt = count_children(ap->firstVersion);
        if (pos < 1 || pos > cnt) { free(seq); return ERROR; }
        Version *removed = remove_child_at(&ap->firstVersion, pos);
        if (!removed) { free(seq); return ERROR; }
        free_version_subtree(removed);
        free(seq);
        return OK;
    } else {
        /* buscar parent y remover hijo pos */
        unsigned int parentLen = seqlen - 1;
        Version *parent = get_node_by_sequence(ap->firstVersion, seq, parentLen);
        if (!parent) { free(seq); return ERROR; }
        unsigned int pos = seq[seqlen - 1];
        unsigned int cnt = count_children(parent->firstChild);
        if (pos < 1 || pos > cnt) { free(seq); return ERROR; }
        Version *removed = remove_child_at(&parent->firstChild, pos);
        if (!removed) { free(seq); return ERROR; }
        free_version_subtree(removed);
        free(seq);
        return OK;
    }
}

/* MostrarVersiones: imprime el nombre, línea en blanco, y las versiones jerárquicamente.
   Formato exacto según consigna.
*/
TipoRet MostrarVersiones(Archivo a) {
    if (!a) return ERROR;
    struct nodo_archivo *ap = (struct nodo_archivo*) a;
    return mostrar_versiones_impl(ap);
}

/* InsertarLinea:
   - version existe
   - version no tenga subversiones (no editable si tiene hijos)
   - nroLinea entre 1 y n+1 donde n es la cantidad de líneas de la version heredada
   - agregar un cambio IL al final de la lista de cambios de esa version
*/
TipoRet InsertarLinea(Archivo &a, char * version, char * linea, unsigned int nroLinea, char * error) {
    if (!a) { if (error) strcpy(error, "Archivo no existe"); return ERROR; }
    unsigned int *seq = NULL, seqlen = 0;
    if (!parse_version_string(version, &seq, &seqlen)) { if (error) strcpy(error, "Version invalida"); return ERROR; }
    struct nodo_archivo *ap = (struct nodo_archivo*) a;
    Version *target = get_node_by_sequence(ap->firstVersion, seq, seqlen);
    if (!target) { free(seq); if (error) strcpy(error, "Version no existe"); return ERROR; }
    if (target->firstChild != NULL) { free(seq); if (error) strcpy(error, "Version tiene subversiones"); return ERROR; }
    /* construir texto actual para conocer n */
    text_array ta;
    build_text_for_version(target->parent ? target->parent : target, &ta);
    /* WARNING: build_text_for_version expects node v and applies changes from root to v.
       To get base text for target we should build for parent (not target) and then apply target changes to validate nroLinea.
       Simpler: we compute text for target by building for target->parent and then apply target->changes so far
    */
    /* recomputar: build text for parent */
    if (target->parent) {
        text_array_free(&ta);
        build_text_for_version(target->parent, &ta);
    } else {
        /* no parent: start empty */
        text_array_free(&ta);
        text_array_init(&ta);
    }
    /* apply existing changes of target to get current text before this insertion */
    Change *c = target->changes;
    while (c) {
        if (c->type == TYPE_IL) text_array_insert(&ta, c->line, c->text ? c->text : "");
        else text_array_delete(&ta, c->line);
        c = c->next;
    }
    unsigned int n = ta.n;
    if (n + 1 < nroLinea || nroLinea < 1) {
        text_array_free(&ta); free(seq);
        if (error) strcpy(error, "Numero de linea invalido");
        return ERROR;
    }
    /* crear nuevo change y anexarlo */
    Change *newc = (Change*) malloc(sizeof(Change));
    newc->type = TYPE_IL;
    newc->line = nroLinea;
    newc->text = strdup_safe(linea ? linea : "");
    newc->next = NULL;
    if (!target->changes) {
        target->changes = newc;
    } else {
        Change *t = target->changes;
        while (t->next) t = t->next;
        t->next = newc;
    }
    text_array_free(&ta);
    free(seq);
    return OK;
}

/* BorrarLinea: análogo a InsertarLinea pero con BL */
TipoRet BorrarLinea(Archivo &a, char * version, unsigned int nroLinea, char * error) {
    if (!a) { if (error) strcpy(error, "Archivo no existe"); return ERROR; }
    unsigned int *seq = NULL, seqlen = 0;
    if (!parse_version_string(version, &seq, &seqlen)) { if (error) strcpy(error, "Version invalida"); return ERROR; }
    struct nodo_archivo *ap = (struct nodo_archivo*) a;
    Version *target = get_node_by_sequence(ap->firstVersion, seq, seqlen);
    if (!target) { free(seq); if (error) strcpy(error, "Version no existe"); return ERROR; }
    if (target->firstChild != NULL) { free(seq); if (error) strcpy(error, "Version tiene subversiones"); return ERROR; }
    /* calcular texto actual */
    text_array ta;
    if (target->parent) build_text_for_version(target->parent, &ta);
    else { text_array_init(&ta); }
    Change *c = target->changes;
    while (c) {
        if (c->type == TYPE_IL) text_array_insert(&ta, c->line, c->text ? c->text : "");
        else text_array_delete(&ta, c->line);
        c = c->next;
    }
    unsigned int n = ta.n;
    if (n < nroLinea || nroLinea < 1) {
        text_array_free(&ta); free(seq);
        if (error) strcpy(error, "Numero de linea invalido");
        return ERROR;
    }
    /* append BL change */
    Change *newc = (Change*) malloc(sizeof(Change));
    newc->type = TYPE_BL;
    newc->line = nroLinea;
    newc->text = NULL;
    newc->next = NULL;
    if (!target->changes) target->changes = newc;
    else {
        Change *t = target->changes;
        while (t->next) t = t->next;
        t->next = newc;
    }
    text_array_free(&ta);
    free(seq);
    return OK;
}

/* MostrarTexto: imprime "nombre - etiqueta" luego una línea en blanco
   y luego cada línea: "n <tab> texto" o "No contiene líneas" si vacio.
*/
TipoRet MostrarTexto(Archivo a, char * version) {
    if (!a) return ERROR;
    unsigned int *seq = NULL, seqlen = 0;
    if (!parse_version_string(version, &seq, &seqlen)) { if (seq) free(seq); return ERROR; }
    struct nodo_archivo *ap = (struct nodo_archivo*) a;
    Version *v = get_node_by_sequence(ap->firstVersion, seq, seqlen);
    if (!v) { free(seq); return ERROR; }
    /* imprimir header */
    printf("%s - ", ap->nombre ? ap->nombre : "");
    /* imprimir etiqueta: reconstruir label a partir de seq */
    for (unsigned int i = 0; i < seqlen; ++i) {
        if (i) printf(".");
        printf("%u", seq[i]);
    }
    printf("\n\n");
    /* construir texto final */
    text_array ta;
    build_text_for_version(v, &ta);
    if (ta.n == 0) {
        printf("No contiene líneas\n");
    } else {
        for (unsigned int i = 0; i < ta.n; ++i) {
            printf("%u\t%s\n", i + 1, ta.lines[i]);
        }
    }
    text_array_free(&ta);
    free(seq);
    return OK;
}

/* MostrarCambios: lista solo los cambios de la versión respecto a su padre en orden */
TipoRet MostrarCambios(Archivo a, char * version) {
    if (!a) return ERROR;
    unsigned int *seq = NULL, seqlen = 0;
    if (!parse_version_string(version, &seq, &seqlen)) { if (seq) free(seq); return ERROR; }
    struct nodo_archivo *ap = (struct nodo_archivo*) a;
    Version *v = get_node_by_sequence(ap->firstVersion, seq, seqlen);
    if (!v) { free(seq); return ERROR; }
    /* imprimir header */
    printf("%s - ", ap->nombre ? ap->nombre : "");
    for (unsigned int i = 0; i < seqlen; ++i) {
        if (i) printf(".");
        printf("%u", seq[i]);
    }
    printf("\n\n");
    if (!v->changes) {
        printf("No se realizaron modificaciones\n");
        free(seq);
        return OK;
    }
    Change *c = v->changes;
    while (c) {
        if (c->type == TYPE_IL) {
            printf("IL\t%u\t%s\n", c->line, c->text ? c->text : "");
        } else {
            printf("BL\t%u\n", c->line);
        }
        c = c->next;
    }
    free(seq);
    return OK;
}

/* Iguales: construir textos y comparar exacto */
TipoRet Iguales(Archivo a, char * version1, char * version2, bool &iguales) {
    iguales = false;
    if (!a) return ERROR;
    unsigned int *s1 = NULL, l1 = 0, *s2 = NULL, l2 = 0;
    if (!parse_version_string(version1, &s1, &l1)) { if (s1) free(s1); return ERROR; }
    if (!parse_version_string(version2, &s2, &l2)) { free(s1); if (s2) free(s2); return ERROR; }
    struct nodo_archivo *ap = (struct nodo_archivo*) a;
    Version *v1 = get_node_by_sequence(ap->firstVersion, s1, l1);
    Version *v2 = get_node_by_sequence(ap->firstVersion, s2, l2);
    if (!v1 || !v2) { free(s1); free(s2); return ERROR; }
    text_array t1, t2;
    build_text_for_version(v1, &t1);
    build_text_for_version(v2, &t2);
    if (t1.n != t2.n) iguales = false;
    else {
        iguales = true;
        for (unsigned int i = 0; i < t1.n; ++i) {
            if (strcmp(t1.lines[i], t2.lines[i]) != 0) { iguales = false; break; }
        }
    }
    text_array_free(&t1);
    text_array_free(&t2);
    free(s1); free(s2);
    return OK;
}

/* VersionIndependiente (opcional):
   - crea una nueva versión de primer nivel (al final) cuyo contenido sea el texto
     resultante de aplicar todos los cambios de la versión dada y sus ancestros.
   - implementamos creando una versión nueva en primer nivel (append) y añadiendo
     IL con todas las líneas del texto final (estado "independiente").
*/
TipoRet VersionIndependiente(Archivo &a, char * version) {
    if (!a) return ERROR;
    unsigned int *seq = NULL, seqlen = 0;
    if (!parse_version_string(version, &seq, &seqlen)) { if (seq) free(seq); return ERROR; }
    struct nodo_archivo *ap = (struct nodo_archivo*) a;
    Version *v = get_node_by_sequence(ap->firstVersion, seq, seqlen);
    if (!v) { free(seq); return ERROR; }
    /* build final text */
    text_array ta;
    build_text_for_version(v, &ta);
    /* create new first-level version at the end */
    unsigned int cnt = count_children(ap->firstVersion);
    Version *newv = insert_child_at(&ap->firstVersion, cnt + 1);
    if (!newv) { text_array_free(&ta); free(seq); return ERROR; }
    newv->parent = NULL;
    /* populate changes as IL 1..n to yield the final text from empty */
    for (unsigned int i = 0; i < ta.n; ++i) {
        Change *c = (Change*) malloc(sizeof(Change));
        c->type = TYPE_IL;
        c->line = i + 1;
        c->text = strdup_safe(ta.lines[i]);
        c->next = NULL;
        if (!newv->changes) newv->changes = c;
        else {
            Change *t = newv->changes;
            while (t->next) t = t->next;
            t->next = c;
        }
    }
    text_array_free(&ta);
    free(seq);
    return OK;
}
