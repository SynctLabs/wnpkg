#include "lnpkg/builder.h"

#include <stdio.h>
#include <stdlib.h>

#include "lnpkg/color.h"

void lnpkg_generate_main() {
  const char* builder =
      "#include <stdlib.h>\n"
      "#include <stdio.h>\n"
      "#include \"lnpkg.h\"\n\n"
      "int main(){\n"
      "  lnpkg_mkdir(\"lnpkg\");\n"
      "  lnpkg_writenode();\n"
      "  lnpkg_writeindexjs();\n"
      "  system(\"chmod +x lnpkg/node\");\n"
      "  system(\"./lnpkg/node lnpkg/index.js\");\n"
      "  lnpkg_cleanup();\n"
      "  return 0;\n"
      "}";

  FILE* app = fopen("lnpkg-build/source/app.c", "w");

  if (app == NULL) {
    RED_PRINT("[Error]: Writing app in lnpkg-build failed.\n");
    exit(1);
  }

  fprintf(app, "%s", builder);
  fclose(app);
  YELLOW_PRINT("[Log]: Application created successfully.\n");
}

void lnpkg_generate_node_s() {
  char builder[2048] =
      ".global _binary_node_start\n"
      ".global _binary_node_end\n"
      ".global _binary_indexjs_start\n"
      ".global _binary_indexjs_end\n"
      "\n"
      ".section .rodata\n"
      "_binary_node_start:\n"
      "    .incbin \"lnpkg-build/source/node\"\n"
      "_binary_node_end:\n"
      "_binary_indexjs_start:\n"
      "    .incbin \"lnpkg-build/source/index.js\"\n"
      "_binary_indexjs_end:\n";
  FILE* node_s = fopen("lnpkg-build/source/node.s", "w");

  if (node_s == NULL) {
    RED_PRINT("[Error]: Writing node.s in lnpkg-build failed.\n");
    fclose(node_s);
    exit(1);
  }

  fprintf(node_s, "%s", builder);
  fclose(node_s);
}

void lnpkg_generate_lnpkg_h() {
  const char builder[] =
      "#include <stdio.h>\n"
      "#include <stdlib.h>\n"
      "\n"
      "extern const unsigned char _binary_node_start[];\n"
      "extern const unsigned char _binary_node_end[];\n"
      "extern const unsigned char _binary_indexjs_start[];\n"
      "extern const unsigned char _binary_indexjs_end[];\n"
      "\n"
      "#include <sys/stat.h>\n"
      "#include <unistd.h>\n"
      "\n"
      "int lnpkg_mkdir(char* dir) {\n"
      "  return mkdir(dir, 0755);\n"
      "}\n"
      "\n"
      "int lnpkg_rmdir(char* dir) {\n"
      "  return rmdir(dir);\n"
      "}\n"
      "\n"
      "static int lnpkg_writenode() {\n"
      "  const unsigned char* node = _binary_node_start;\n"
      "  size_t node_len = (size_t)(_binary_node_end - _binary_node_start);\n"
      "\n"
      "  FILE* node_f = fopen(\"lnpkg/node\", \"wb\");\n"
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
      "    fprintf(stderr, \"Error: failed to write full node (wrote %zu of "
      "%zu bytes)\\n\",\n"
      "            written, node_len);\n"
      "    return 1;\n"
      "  }\n"
      "\n"
      "  return 0;\n"
      "}\n"
      "\n"
      "static int lnpkg_writeindexjs() {\n"
      "  const unsigned char* indexjs = _binary_indexjs_start;\n"
      "  size_t indexjs_len = (size_t)(_binary_indexjs_end - "
      "_binary_indexjs_start);\n"
      "\n"
      "  FILE* indexjs_f = fopen(\"lnpkg/index.js\", \"wb\");\n"
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
      "    fprintf(stderr, \"Error: failed to write full indexjs (wrote %zu of "
      "%zu bytes)\\n\",\n"
      "            written, indexjs_len);\n"
      "    return 1;\n"
      "  }\n"
      "\n"
      "  return 0;\n"
      "}\n"
      "\n"
      "void lnpkg_cleanup() {\n"
      "  remove(\"lnpkg/index.js\");\n"
      "  remove(\"lnpkg/node\");\n"
      "  lnpkg_rmdir(\"lnpkg\");\n"
      "}";

  FILE* lnpkg_h;

  lnpkg_h = fopen("lnpkg-build/source/lnpkg.h", "w");

  if (!lnpkg_h) {
    RED_PRINT("[Error]: Writing lnpkg.h in lnpkg-build failed.\n");
    exit(1);
  }

  fprintf(lnpkg_h, "%s", builder);
  fclose(lnpkg_h);
}

void lnpkg_generate_node() {
  char cmd[2048];
  int is_exe = 0;

  const char* PREFIX = getenv("PREFIX");
  if (PREFIX == NULL) {
    RED_PRINT("[Error]:  Failed to get NODE Bin\n");
    return;
  }
  sprintf(cmd, "cp %s/bin/node lnpkg-build/source/node", PREFIX);

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