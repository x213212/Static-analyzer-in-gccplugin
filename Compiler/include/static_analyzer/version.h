#ifndef STATIC_ANALYZER_VERSION_H
#define STATIC_ANALYZER_VERSION_H

// Keep the plugin ABI metadata and every machine/human-readable report on one
// release identifier.  This is a source-level version; GCC's
// plugin_default_version_check separately enforces the compiler ABI match.
#define STATIC_ANALYZER_VERSION "1.0.0-rc3"

#endif
