
if get_option('man-pages').enabled()
  manpages = ['glib-gettextize', 'gtester', 'gtester-report']
  foreach page : manpages
    custom_target(page + '-man',
      input: page + '.rst',
      output: page + '.1',
      command: [
        rst2man,
        rst2man_flags,
        '@INPUT@',
      ],
      capture: true,
      install: true,
      install_dir: man1_dir,
      install_tag: 'doc',
    )
  endforeach
endif

if get_option('documentation')
  # GVariant specification is currently standalone
  rst2html5 = find_program('rst2html5', 'rst2html5.py', required: false)

  if rst2html5.found()
    spec_path = docs_dir

    figures = files(
      'gvariant-byte-boundaries.svg',
      'gvariant-integer-and-string-structure.svg',
      'gvariant-integer-array.svg',
      'gvariant-string-array.svg',
    )

    custom_target('gvariant-specification-1.0',
      input: 'gvariant-specification-1.0.rst',
      output: 'gvariant-specification-1.0.html',
      command: [
        rst2html5,
        '@INPUT@',
      ],
      capture: true,
      install: true,
      install_dir: spec_path,
      install_tag: 'doc',
      depend_files: figures,
    )

    install_data(figures,
      install_dir : spec_path,
      install_tag : 'doc',
    )
  endif
endif

if get_option('documentation') and enable_gir
  expand_content_files = [
    'atomic.md',
    'base64.md',
    'building.md',
    'character-set.md',
    'checked-math.md',
    'compiling.md',
    'cross-compiling.md',
    'datalist-and-dataset.md',
    'error-reporting.md',
    'file-utils.md',
    'gvariant-format-strings.md',
    'gvariant-text-format.md',
    'i18n.md',
    'logging.md',
    'main-loop.md',
    'memory.md',
    'memory-slices.md',
    'numerical.md',
    'random.md',
    'reference-counting.md',
    'running.md',
    'testing.md',
    'threads.md',
    'threads-deprecated.md',
    'markup.md',
    'misc-utils.md',
    'goption.md',
    'host-utils.md',
    'data-structures.md',
    'programming.md',
    'resources.md',
    'shell.md',
    'spawn.md',
    'string-utils.md',
    'types.md',
    'unicode.md',
    'unix.md',
    'uuid.md',
    'version.md',
    'warnings.md',
    'windows.md',
  ]

  glib_toml = configure_file(input: 'glib.toml.in', output: 'glib.toml', configuration: toml_conf)

  custom_target('glib-docs',
    input: [ glib_toml, glib_gir[0] ],
    output: 'glib',
    command: [
      gidocgen,
      'generate',
      gidocgen_common_args,
      '--config=@INPUT0@',
      '--output-dir=@OUTPUT@',
      '--content-dir=@0@'.format(meson.current_source_dir()),
      '@INPUT1@',
    ],
    build_by_default: true,
    depend_files: expand_content_files,
    install: true,
    install_dir: docs_dir,
    install_tag: 'doc',
  )
endif
