#include <ruby/ruby.h>

/* internal.h */
enum imemo_type {
    imemo_none = 0,
    imemo_cref = 1,
    imemo_svar = 2,
    imemo_throw_data = 3,
    imemo_ifunc = 4,
    imemo_memo = 5,
    imemo_ment = 6,
    imemo_iseq = 7,
    imemo_mask = 0x07
};

static inline enum imemo_type
imemo_type(VALUE imemo)
{
    return (RBASIC(imemo)->flags >> FL_USHIFT) & imemo_mask;
}
/* end */

/* ccan/list/list.h */
struct list_node
{
	struct list_node *next, *prev;
};
/* */

/* vm_core.h */
typedef struct rb_iseq_struct {
    VALUE flags;
    VALUE reserved1;
    // struct rb_iseq_constant_body *body;
    void *body;

    union { /* 4, 5 words */
	// struct iseq_compile_data *compile_data; /* used at compile time */
	void *compile_data; /* used at compile time */

	struct {
	    VALUE obj;
	    int index;
	} loader;
    } aux;
} rb_iseq_t;

typedef struct rb_control_frame_struct {
    const VALUE *pc;		/* cfp[0] */
    VALUE *sp;			/* cfp[1] */
    const rb_iseq_t *iseq;	/* cfp[2] */
    VALUE flag;			/* cfp[3] */
    VALUE self;			/* cfp[4] / block[0] */
    VALUE *ep;			/* cfp[5] / block[1] */
    const rb_iseq_t *block_iseq;/* cfp[6] / block[2] */
    VALUE proc;			/* cfp[7] / block[3] */

#if VM_DEBUG_BP_CHECK
    VALUE *bp_check;		/* cfp[8] */
#endif
} rb_control_frame_t;

typedef struct rb_block_struct {
    VALUE self;			/* share with method frame if it's only block */
    VALUE *ep;			/* share with method frame if it's only block */
    const rb_iseq_t *iseq;
    VALUE proc;
} rb_block_t;

typedef struct {
    rb_block_t block;
    int8_t safe_level;		/* 0..1 */
    int8_t is_from_method;	/* bool */
    int8_t is_lambda;		/* bool */
} rb_proc_t;

// partial
typedef struct rb_thread_struct {
    struct list_node vmlt_node;
    VALUE self;
    //rb_vm_t *vm;
    void *vm;

    /* execution information */
    VALUE *stack;		/* must free, must mark */
    size_t stack_size;          /* size in word (byte size / sizeof(VALUE)) */
    rb_control_frame_t *cfp;
    int safe_level;
} rb_thread_t;
/* end */

/* proc.c */
static void
proc_mark(void *ptr)
{
    rb_proc_t *proc = ptr;
    rb_gc_mark(proc->block.proc);
    rb_gc_mark(proc->block.self);
    rb_gc_mark(proc->block.ep[1]);
}

typedef struct {
    rb_proc_t basic;
    VALUE env[3]; /* me, specval, envval */
} cfunc_proc_t;

static size_t
proc_memsize(const void *ptr)
{
    const rb_proc_t *proc = ptr;
    if (proc->block.ep == ((const cfunc_proc_t *)ptr)->env+1)
	return sizeof(cfunc_proc_t);
    return sizeof(rb_proc_t);
}

static const rb_data_type_t proc_data_type = {
    "proc",
    {
	proc_mark,
	RUBY_TYPED_DEFAULT_FREE,
	proc_memsize,
    },
    0, 0, RUBY_TYPED_FREE_IMMEDIATELY
};
/* end */

static VALUE
callee(void)
{
    rb_thread_t *th = (rb_thread_t *)RTYPEDDATA_DATA(rb_thread_current());
    rb_control_frame_t *cfp = th->cfp + 1; // previous frame
    const rb_iseq_t *iseq = cfp->iseq;

    if (RB_TYPE_P((VALUE)(iseq), T_IMEMO) && imemo_type((VALUE)iseq) == imemo_iseq) {
        rb_proc_t *proc;
        VALUE procval = TypedData_Make_Struct(rb_cProc, rb_proc_t, &proc_data_type, proc);
        proc->block.self = cfp->self;
        proc->block.ep = cfp->ep;
        proc->block.iseq = iseq;
        proc->block.proc = procval;
        proc->safe_level = th->safe_level;
        proc->is_from_method = 0;
        proc->is_lambda = 0; // TODO: ??
        return procval;
    } else {
        rb_raise(rb_eArgError, "caller normal iseq?");
    }
}

void
Init_callee(void)
{
    rb_define_global_function("callee", callee, 0);
}
