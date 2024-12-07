#include <libgccjit.h>

#include <stdlib.h>
#include <stdio.h>

static void create_code(gcc_jit_context *ctxt) {
  gcc_jit_type *int_type = gcc_jit_context_get_type(ctxt, GCC_JIT_TYPE_INT);
  gcc_jit_type *const_char_ptr_type = gcc_jit_context_get_type(ctxt, GCC_JIT_TYPE_CONST_CHAR_PTR);
  gcc_jit_function *func = gcc_jit_context_new_function(ctxt, NULL, GCC_JIT_FUNCTION_EXPORTED,
      int_type, "main", 0, NULL, 0);

  gcc_jit_param *param_format = gcc_jit_context_new_param(ctxt, NULL, const_char_ptr_type, "format");
  gcc_jit_function *printf_func = gcc_jit_context_new_function(ctxt, NULL, GCC_JIT_FUNCTION_IMPORTED,
    gcc_jit_context_get_type(ctxt, GCC_JIT_TYPE_INT), "printf", 1, &param_format, 0);
  gcc_jit_rvalue *args[1];
  args[0] = gcc_jit_context_new_string_literal(ctxt, "hello\n");

  gcc_jit_block *block = gcc_jit_function_new_block(func, NULL);

  gcc_jit_block_add_eval(block, NULL, gcc_jit_context_new_call(ctxt, NULL, printf_func, 1, args));
  gcc_jit_rvalue *zero = gcc_jit_context_zero(ctxt, int_type);
  gcc_jit_block_end_with_return(block, NULL, zero);
}

int main(int argc, char **argv) {
    gcc_jit_context *ctxt;

    ctxt = gcc_jit_context_acquire();
    if(!ctxt) {
        fprintf(stderr, "NULL ctxt");
        exit(1);
    }

    gcc_jit_context_set_bool_option(ctxt, GCC_JIT_BOOL_OPTION_DUMP_GENERATED_CODE, 0);

    create_code(ctxt);

    gcc_jit_context_compile_to_file(ctxt, GCC_JIT_OUTPUT_KIND_EXECUTABLE, "smoke_test");

    gcc_jit_context_release(ctxt);
    return 0;
}
