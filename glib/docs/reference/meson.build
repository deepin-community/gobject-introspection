if get_option('documentation') and enable_gir
  gidocgen_dep = dependency('gi-docgen', version: '>= 2023.1',
                          fallback: ['gi-docgen', 'dummy_dep'],
                          required: true)

  toml_conf = configuration_data()
  toml_conf.set('VERSION', meson.project_version())

  gidocgen = find_program('gi-docgen', required: true)

  gidocgen_common_args = [
    '--quiet',
    '--no-namespace-dir',
    '--fatal-warnings',
    '--add-include-path=@0@'.format(meson.current_build_dir() / '../../introspection'),
  ]

  docs_dir = glib_datadir / 'doc' / 'glib-2.0'
endif

subdir('glib')
subdir('gmodule')
subdir('gobject')
subdir('gio')
subdir('girepository')
