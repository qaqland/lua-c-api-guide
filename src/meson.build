examples = [
  '00-overview',
  '01-stack',
  '02-types',
  '03-call-lua',
  '04-call-c',
  '05-tables',
  '06-errors',
  '07-userdata',
  '08-metatable',
  '09-coroutine',
  '10-gc',
  '11-upvalue',
  '12-state',
  '13-arith',
  '14-load',
  '15-ref',
  '16-buffer',
  '17-debug',
  '18-warn',
  '19-stdlib',
]

foreach name : examples
  exe = executable(name, name + '.c', dependencies : lua_dep)
  test(name, exe)
endforeach
