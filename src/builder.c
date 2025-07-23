#include "wnpkg/builder.h"

#include <stdio.h>
#include <stdlib.h>

#include "wnpkg/color.h"
#include "wnpkg/plat.h"

void wnpkg_generate_main() {
#ifdef WNPKG_WINDOWS
  const char* builder =
      "#include <stdlib.h>\n"
      "#include <stdio.h>\n"
      "#include \"wnpkg.h\"\n\n"
      "int main(){\n"
      "  wnpkg_mkdir(\"wnpkg\");\n"
      "  wnpkg_writenode();\n"
      "  wnpkg_writeindexjs();\n"
      "  system(\"wnpkg\\\\node.exe wnpkg\\\\index.js\");\n"
      "  wnpkg_cleanup();\n"
      "  return 0;\n"
      "}";

  FILE* app = fopen("wnpkg-build\\source\\app.c", "w");
#else
  const char* builder =
      "#include <stdlib.h>\n"
      "#include <stdio.h>\n"
      "#include \"wnpkg.h\"\n\n"
      "int main(){\n"
      "  wnpkg_mkdir(\"wnpkg\");\n"
      "  wnpkg_writenode();\n"
      "  wnpkg_writeindexjs();\n"
      "  system(\"chmod +x wnpkg/node\");\n"
      "  system(\"./wnpkg/node wnpkg/index.js\");\n"
      "  wnpkg_cleanup();\n"
      "  return 0;\n"
      "}";

  FILE* app = fopen("wnpkg-build/source/app.c", "w");
#endif

  if (app == NULL) {
    RED_PRINT("[Error]: Writing app in wnpkg-build failed.\n");
    exit(1);
  }

  fprintf(app, "%s", builder);
  fclose(app);
  YELLOW_PRINT("[Log]: Application created successfully.\n");
}

void wnpkg_generate_node_s() {
#ifdef WNPKG_WINDOWS
  char builder[1024] =
      ".global _binary_node_bin_start\n"
      ".global _binary_node_bin_end\n"
      ".global _binary_indexjs_start\n"
      ".global _binary_indexjs_end\n"
      "\n"
      ".section .rodata\n"
      "_binary_node_bin_start:\n"
      "    .incbin \"wnpkg-build\\source\\node.exe\"\n"
      "_binary_node_bin_end:\n"
      "_binary_indexjs_start:\n"
      "    .incbin \"wnpkg-build\\source\\index.js\"\n"
      "_binary_indexjs_end:\n";
  FILE* node_s = fopen("wnpkg-build\\source\\node.s", "w");
#else
  char builder[2048] =
      ".global _binary_node_start\n"
      ".global _binary_node_end\n"
      ".global _binary_indexjs_start\n"
      ".global _binary_indexjs_end\n"
      "\n"
      ".section .rodata\n"
      "_binary_node_start:\n"
      "    .incbin \"wnpkg-build/source/node\"\n"
      "_binary_node_end:\n"
      "_binary_indexjs_start:\n"
      "    .incbin \"wnpkg-build/source/index.js\"\n"
      "_binary_indexjs_end:\n";
  FILE* node_s = fopen("wnpkg-build/source/node.s", "w");
#endif

  if (node_s == NULL) {
    RED_PRINT("[Error]: Writing node.s in wnpkg-build failed.\n");
    fclose(node_s);
    exit(1);
  }

  fprintf(node_s, "%s", builder);
  fclose(node_s);
}

void wnpkg_generate_wnpkg_h() {
  const char builder[] =
      "#include <stdio.h>\n"
      "#include <stdlib.h>\n"
      "\n"
      "extern const unsigned char _binary_node_start[];\n"
      "extern const unsigned char _binary_node_end[];\n"
      "extern const unsigned char _binary_indexjs_start[];\n"
      "extern const unsigned char _binary_indexjs_end[];\n"
      "\n"
      "#if defined(__WIN64) || defined(__WIN32)\n"
      "#define Win\n"
      "#endif\n"
      "\n"
      "#ifdef Win\n"
      "#include <direct.h>\n"
      "#else\n"
      "#include <sys/stat.h>\n"
      "#include <unistd.h>\n"
      "#endif\n"
      "\n"
      "int wnpkg_mkdir(char *dir) {\n"
      "#ifdef WNPKG_WINDOWS\n"
      "  return _mkdir(dir);\n"
      "#else\n"
      "  return mkdir(dir, 0755);\n"
      "#endif\n"
      "}\n"
      "\n"
      "int wnpkg_rmdir(char *dir) {\n"
      "#ifdef Win\n"
      "  return _rmdir(dir);\n"
      "#else\n"
      "  return rmdir(dir);\n"
      "#endif\n"
      "}\n"
      "\n"
      "static int wnpkg_writenode() {\n"
      "  const unsigned char* node = _binary_node_start;\n"
      "  size_t node_len = (size_t)(_binary_node_end - _binary_node_start);\n"
      "\n"
      "#ifdef Win\n"
      "  FILE* node_f = fopen(\"wnpkg/node.exe\", \"wb\");\n"
      "#else\n"
      "  FILE* node_f = fopen(\"wnpkg/node\", \"wb\");\n"
      "#endif\n"
      "\n"
      "  if (!node_f) {\n"
      "    perror(\"fopen\");\n"
      "    return 1;\n"
      "  }\n"
      "\n"
      "  size_t written = fwrite(node, 1, node_len, node_f);\n"
      "  fclose(node_f);\n"
      "\n"
      "  if (written != node_len) {\n"
      "#ifdef Win\n"
      "    fprintf(stderr, \"Error: failed to write full node.exe (wrote %zu "
      "of %zu bytes)\\n\",\n"
      "            written, node_len);\n"
      "#else\n"
      "    fprintf(stderr, \"Error: failed to write full node (wrote %zu of "
      "%zu bytes)\\n\",\n"
      "            written, node_len);\n"
      "#endif\n"
      "    return 1;\n"
      "  }\n"
      "\n"
      "  return 0;\n"
      "}\n"
      "\n"
      "static int wnpkg_writeindexjs() {\n"
      "  const unsigned char* indexjs = _binary_indexjs_start;\n"
      "  size_t indexjs_len = (size_t)(_binary_indexjs_end - "
      "_binary_indexjs_start);\n"
      "\n"
      "  FILE* indexjs_f = fopen(\"wnpkg/index.js\", \"wb\");\n"
      "\n"
      "  if (!indexjs_f) {\n"
      "    perror(\"fopen\");\n"
      "    return 1;\n"
      "  }\n"
      "\n"
      "  size_t written = fwrite(indexjs, 1, indexjs_len, indexjs_f);\n"
      "  fclose(indexjs_f);\n"
      "\n"
      "  if (written != indexjs_len) {\n"
      "#ifdef Win\n"
      "    fprintf(stderr, \"Error: failed to write full indexjs.exe (wrote "
      "%zu of %zu bytes)\\n\",\n"
      "            written, indexjs_len);\n"
      "#else\n"
      "    fprintf(stderr, \"Error: failed to write full indexjs (wrote %zu of "
      "%zu bytes)\\n\",\n"
      "            written, indexjs_len);\n"
      "#endif\n"
      "    return 1;\n"
      "  }\n"
      "\n"
      "  return 0;\n"
      "}\n"
      "\n"
      "void wnpkg_cleanup() {\n"
      "  remove(\"wnpkg/index.js\");\n"
      "#ifdef Win\n"
      "  remove(\"wnpkg/node.exe\");\n"
      "#else\n"
      "  remove(\"wnpkg/node\");\n"
      "#endif\n"
      "  wnpkg_rmdir(\"wnpkg\");\n"
      "}";

  FILE* wnpkg_h;

#ifdef WNPKG_WINDOWS
  wnpkg_h = fopen("wnpkg-build\\source\\wnpkg.h", "w");
#else
  wnpkg_h = fopen("wnpkg-build/source/wnpkg.h", "w");
#endif

  if (!wnpkg_h) {
    RED_PRINT("[Error]: Writing wnpkg.h in wnpkg-build failed.\n");
    exit(1);
  }

  fprintf(wnpkg_h, "%s", builder);
  fclose(wnpkg_h);
}

void wnpkg_generate_node() {
  char cmd[2048];
  int is_exe = 0;
#ifdef WNPKG_WINDOWS
  is_exe = 1;
  sprintf(cmd,
          "copy \"\\Program Files\\nodejs\\node.exe\" "
          "wnpkg-build\\source\\node.exe");
#else
  const char* PREFIX = getenv("PREFIX");
  if (PREFIX == NULL) {
    RED_PRINT("[Error]:  Failed to get NODE Bin\n");
    return;
  }
  sprintf(cmd, "cp %s/bin/node wnpkg-build/source/node", PREFIX);
#endif

  if (system(cmd) == 0) {
    YELLOW_PRINT("[Log]: added %s in source folder.\n",
                 (is_exe) ? "node.exe" : "node");
  } else {
    RED_PRINT(
        "[Error]:  Unable to move %s file in target folder, file may not "
        "exist.\n",
        (is_exe) ? "node.exe" : "node");
  }
}