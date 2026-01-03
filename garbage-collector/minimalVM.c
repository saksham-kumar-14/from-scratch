#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define STACK_SIZE 256
#define INITIAL_GC_THRESHOLD 100

typedef enum {
    OBJ_INT,
    OBJ_PAIR
} ObjectType;

typedef struct sObject {
    ObjectType type;
    unsigned char marked;

    struct sObject* next;

    union {
        int value; // for OBJ_INT

        struct {
            struct sObject* head;
            struct sObject* tail;
        }; // for OBJ_PAIR
    };
} Object;


typedef struct {
    Object* stack[STACK_SIZE];
    int stackSize;

    Object* head;

    int numObjects;
    int maxObjects;
} VM;

void mark(Object* object);
void markAll(VM* vm);
void sweep(VM* vm);
void garbageCollector(VM* vm);

// init a new virtual machine
VM* newVM() {
    VM* vm = malloc(sizeof(VM));
    vm->stackSize = 0;
    vm->numObjects = 0;
    vm->maxObjects = INITIAL_GC_THRESHOLD;
    vm->head = NULL;
    return vm;
}

// VM stack functions
void push(VM* vm, Object* value) {
    assert(vm->stackSize < STACK_SIZE && "STACK OVERFLOW!");
    vm->stack[vm->stackSize++] = value;
}

Object* pop(VM* vm) {
    assert(vm->stackSize > 0 && "STACK UNDERFLOW!");
    return vm->stack[--vm->stackSize];
}

// helper function for creating objects
Object* newObject(VM* vm, ObjectType type) {
    if (vm->numObjects >= vm->maxObjects) {
        garbageCollector(vm);
    }

    Object* object = malloc(sizeof(Object));
    object->type = type;
    object->marked = 0;

    object->next = vm->head;
    vm->head = object;

    vm->numObjects++;
    return object;
}

void pushInt(VM* vm, int value) {
    Object* object = newObject(vm, OBJ_INT);
    object->value = value;
    push(vm, object);
}

Object* pushPair(VM* vm) {
    Object* object = newObject(vm, OBJ_PAIR);
    object->tail = pop(vm);
    object->head = pop(vm);

    push(vm, object);
    return object;
}

// collector

void mark(Object* object) {
    if (object == NULL || object->marked) return;

    object->marked = 1;

    if (object->type == OBJ_PAIR) {
        mark(object->head);
        mark(object->tail);
    }
}

void markAll(VM* vm) {
    for (int i = 0; i < vm->stackSize; ++i) {
        mark(vm->stack[i]);
    }
}

void sweep(VM* vm) {
    Object** object = &vm->head;
    while (*object) {
        if (!(*object)->marked) {
            Object* unreached = *object;

            *object = unreached->next;
            free(unreached);
            vm->numObjects--;
        } else {
            (*object)->marked = 0;
            object = &(*object)->next;
        }
    }
}

void garbageCollector(VM* vm) {
    markAll(vm);
    sweep(vm);
    vm->maxObjects = vm->numObjects * 2;
}

// Testing
int main() {
    VM* vm = newVM();

    for (int i = 0; i < 1000; i++) {
        pushInt(vm, i);
    }

    for (int i = 0; i < 500; i++) {
        pushPair(vm);
    }

    garbageCollector(vm);
    printf("Garbage collection done. Remaining objects: %d\n", vm->numObjects);
    return 0;
}
