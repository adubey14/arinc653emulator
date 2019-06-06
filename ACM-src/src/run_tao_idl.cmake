set(ENV{PATH} "$ENV{ACE_ROOT}/bin:$ENV{PATH}")
set(ENV{LD_LIBRARY_PATH} "$ENV{ACE_ROOT}'lib:$ENV{LD_LIBRARY_PATH}")

execute_process(
  COMMAND tao_idl -aw -SS ACM_IDL_TYPES.idl
  WORKING_DIRECTORY  ${TOP}/include/ACM/USER/
 )

 FILE(WRITE ${TOP}/include/ACM/USER/ACM_IDL_TYPES.h "#include <ACM/USER/ACM_IDL_TYPESC.h>\n")
