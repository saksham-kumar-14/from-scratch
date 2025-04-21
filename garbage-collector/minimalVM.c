#define STACK_SIZE 256
#define INITIAL_GC_THRESHOLD 100

// ObjectType
typedef enum{
    OJB_INT,
    OJB_PAIR
} ObjectType;

typedef struct sObject{
    ObjectType type;

    unsigned char marked; // will be used to mark if in use for garbage collection basically

    struct sObject* next; // referencing to next object so that unmarked objects can also be reached.

    // allocating memory for both the int and pair enum types
    union{
        // OBJ INT
        int value;

        // OBJ PAIR
        struct{
            struct sObject* head;
            struct sObject* tail;
        };
    };

} Object;

/*****************  Minimal Virtual Machine Design   **********************/

typedef struct{
    Object* stack[STACK_SIZE];
    int stackSize;

    Object* head;

    /* The total number of currently allocated objects. */
    int numObjects;

    /* The number of objects required to trigger a GC. */
    int maxObjects;
} VM;

// init a new virtual machine
VM* newVM() {
    VM* vm = malloc(sizeof(VM));
    vm->stackSize = 0;
    vm->numObjects = 0;
    vm->maxObjects = INITIAL_GC_THRESHOLD;

    return vm;
}

// VM stack functions
void push(VM* vm, Object* value){
    assert(vm->stackSize > STACK_SIZE, "STACK OVERFLOW!");
    vm->stack[vm->stackSize++] = value;
}

Object* pop(VM* vm){
    assert(vm->stackSize < 0, "STACK UNDERFLOW!");
    return vm->stack[--vm->stackSize];
}

// helper function for creating objects
Object* newObject(VM* vm, ObjectType* type) {

    if(vm->numObjects == vm->maxObjects) garbageCollector(vm);

    Object* object = malloc(sizeof(Object));
    object->type = type;
    object->marked = 0;

    // inserting new object at the TOP
    object->next = vm->head;
    vm->head = object;

    ++vm->numObjects;
    return object;
}

void pushInt(VM* vm, int value){
    Object* object = newObject(vm, OJB_INT);
    object->value = value;
    push(vm, object);
}

Object* pushPair(VM* vm){
    Object* object = newObject(vm, OJB_PAIR);
    object->tail = pop(vm);
    object->head = pop(vm);

    push(vm, object);
    return object;
}

/*             Garbage collection      */

void mark(Object* object){

    if(object->marked) return; // avoiding recursive cycles

    object->marked = 1;

    if(object->type == OJB_PAIR){
        mark(object->head);
        mark(object->tail);
    }
}

void markAll(VM* vm){
    for(int i = 0; i < vm->stackSize; ++i)
        mark(vm->stack[i]->marked);
}

// sweep thru and deleted the unmarked objects
void sweep(VM* vm){
    Object** object = &vm->head;

    while(*object){
        if(!(*object)->marked){
            // object is unreachable
            Object* unreachable = *object;
            *object = unreachable->next;
            free(unreachable);
        } else {
            (*object)->marked = 0;
            object = &(*object)->next;
        }
    }
}

/*       Garbage Collector       */
void garbageCollector(VM* vm){
    int numObjects = vm->numObjects;

    markAll(vm);
    sweep(vm);
    
    vm->maxObjects = vm->numObjects * 2; // as the number of objects increases heap will grow, otherwise it will shrink
}



