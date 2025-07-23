#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lnpkg/builder.h"
#include "lnpkg/color.h"
#include "lnpkg/file.h"

#define LNPKG_BUILD_DIR "lnpkg-build"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Please provide project folder.\n");
    return 1;
  }

  char path[1024];
  char app_name[1024];
  char config_path[1024];
  int letter = 0;
  char c;

  YELLOW_PRINT("[Log]: Making Builder folder...\n");

  lnpkg_rmr(LNPKG_BUILD_DIR);

  // Create build folder
  if (lnpkg_mkdir(LNPKG_BUILD_DIR) == 0) {
    YELLOW_PRINT("[Log]: Builder folder (lnpkg-build) created successfully.\n");
  } else {
    printf(
        "[Error]: Builder folder (lnpkg-build) may already exist or failed to "
        "create.\n");
    return 1;
  }

  // Create source subfolder
  YELLOW_PRINT("[Log]: Making Source folder...\n");
  if (lnpkg_mkdir("lnpkg-build/source") == 0) {
    YELLOW_PRINT(
        "[Log]: Source folder (lnpkg-build/source) created successfully.\n");
  } else {
    printf(
        "[Error]: Source folder (lnpkg-build) may already exist or failed to "
        "create.\n");
    return 1;
  }

  lnpkg_generate_main();
  lnpkg_generate_node();
  lnpkg_generate_node_s();
  lnpkg_generate_lnpkg_h();

  char cmd[2048];

#define RESET_CMD() memset(cmd, 0, sizeof(cmd))

  // Copy index.js into build folder
  sprintf(cmd, "cp %s/index.js lnpkg-build/source/index.js", argv[1]);
  sprintf(config_path, "%s/lnpkg_config", argv[1]);

  if (system(cmd) == 0) {
    YELLOW_PRINT("[Log]: Added index.js in source folder.\n");
  } else {
    printf(
        "[Error]: Unable to move index.js file in target folder, file may not "
        "exist.\n");
    return 1;
  }

  // Copy package.json into build folder
  sprintf(cmd, "cp %s/package.json lnpkg-build/source/package.json", argv[1]);

  if (system(cmd) == 0) {
    YELLOW_PRINT("[Log]: Added package.json in source folder.\n");
  } else {
    printf(
        "[Error]: Unable to move package.json file in target folder, file may "
        "not exist.\n");
    return 1;
  }

  // reset cmd
  RESET_CMD();

  // Copy node_modules into build folder
  char folder[1024];
  if (lnpkg_havedir(folder)) {
    snprintf(folder, sizeof(folder), "%s/node_modules", argv[1]);
    sprintf(cmd, "cp -rf %s/node_modules lnpkg-build/source/node_modules",
            argv[1]);
  }

  if (system(cmd) == 0) {
    YELLOW_PRINT("[Log]: Added node modules in source folder.\n");
  } else {
    printf(
        "[Error]: Unable to move node modules file in target folder, file may "
        "not exist.\n");
    return 1;
  }

  // Read app name from configuration file
  FILE* fname = fopen(config_path, "r");
  if (fname == NULL) {
    RED_PRINT("[Error]: Config file (lnpkg_config) not found.\n");
    fclose(fname);
    exit(1);
  }

  fgets(app_name, sizeof(app_name), fname);
  app_name[strcspn(app_name, "\r\n")] = 0;

  // Parse app name until delimiter
  letter = 0;
  while (app_name[letter] != ';')
    letter++;
  app_name[letter] = '\0';

  // Build executable using GCC
  RESET_CMD();
  snprintf(cmd, sizeof(cmd),
           "gcc lnpkg-build/source/app.c lnpkg-build/source/node.s -o "
           "lnpkg-build/%s",
           app_name);

  if (system(cmd) == 0) {
    YELLOW_PRINT("[Log]: Added application executable in build folder.\n");
  } else {
    RED_PRINT(
        "[Error]: Unable to add application executable in build folder.\n");
  }

  fclose(fname);

  GREEN_PRINT("[Success]: Project compiled successfully.\n");

  return 0;
}