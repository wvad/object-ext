{
  'targets': [{
    'target_name': 'addon',
    'sources': ['addon.cc'],
    'include_dirs': [],
    'cflags!': ['-fno-exceptions'],
    'cflags_cc!': ['-fno-exceptions', '-std=c++17', "-fno-rtti"]
  }]
}