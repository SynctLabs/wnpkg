#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wnpkg/builder.h"
#include "wnpkg/color.h"
#include "wnpkg/file.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Please provide project folder.\n");
    return 1;
  }

  char path[1024];
  char icon[1024];
  char app_name[1024];
  char config_path[1024];
  int letter = 0;
  char c;
  int use_icon = 0;

  YELLOW_PRINT("[Log]: Making Builder folder...\n");

  // Remove old build folder
  if (wnpkg_rmdir("wnpkg-build") == 0) {
    YELLOW_PRINT(
        "[Log]: Old builder folder (wnpkg-build) removed successfully.\n");
  }

  // Create build folder
  if (wnpkg_mkdir("wnpkg-build") == 0) {
    YELLOW_PRINT("[Log]: Builder folder (wnpkg-build) created successfully.\n");
  } else {
    printf(
        "[Error]:  Builder folder (wnpkg-build) may already exist or failed to "
        "create.\n");
    return 1;
  }

  // Create source subfolder
  YELLOW_PRINT("[Log]: Making Source folder...\n");
  if (wnpkg_mkdir("wnpkg-build/source") == 0) {
    YELLOW_PRINT(
        "[Log]: Source folder (wnpkg-build/source) created successfully.\n");
  } else {
    printf(
        "[Error]:  Source folder (wnpkg-build) may already exist or failed to "
        "create.\n");
    return 1;
  }

  wnpkg_generate_main();
  wnpkg_generate_node();
  wnpkg_generate_node_s();
  wnpkg_generate_wnpkg_h();

  char cmd[2048];

#define RESET_CMD() memset(cmd, 0, sizeof(cmd))

  // Copy index.js into build folder
  sprintf(cmd, "cp %s/index.js wnpkg-build/source/index.js", argv[1]);
  sprintf(config_path, "%s/wnpkg_config", argv[1]);

  if (system(cmd) == 0) {
    YELLOW_PRINT("[Log]: added index.js in source folder.\n");
  } else {
    printf(
        "[Error]:  Unable to move index.js file in target folder, file may not "
        "exist.\n");
    return 1;
  }

  // Copy package.json into build folder
  sprintf(cmd, "cp %s/package.json wnpkg-build/source/package.json", argv[1]);

  if (system(cmd) == 0) {
    YELLOW_PRINT("[Log]: added package.json in source folder.\n");
  } else {
    printf(
        "[Error]:  Unable to move package.json file in target folder, file may "
        "not exist.\n");
    return 1;
  }

  // reset cmd
  RESET_CMD();

  // Copy node_modules into build folder
  char folder[1024];
  if (wnpkg_havedir(folder)) {
    snprintf(folder, sizeof(folder), "%s/node_modules", argv[1]);
    sprintf(cmd, "cp -rf %s/node_modules wnpkg-build/source/node_modules",
            argv[1]);
  }

  if (system(cmd) == 0) {
    YELLOW_PRINT("[Log]: added node modules in source folder.\n");
  } else {
    printf(
        "[Error]:  Unable to move node modules file in target folder, file may "
        "not exist.\n");
    return 1;
  }

  // Read app name and icon from configuration file
  FILE* fname = fopen(config_path, "r");
  if (fname == NULL) {
    RED_PRINT("[Error]:  Config file (wnpkg_config) not found.\n");
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

  fgets(icon, sizeof(icon), fname);
  icon[strcspn(icon, "\r\n")] = 0;

  // Parse icon name until delimiter
  letter = 0;
  while (icon[letter] != ';')
    letter++;
  icon[letter] = '\0';

  // Check if user wants default icon
  if (strcmp(icon, "*") == 0) {
    YELLOW_PRINT("[Log]: Default icon defined successfully.\n");
  } else {
    // Copy user-defined icon into source folder
    RESET_CMD();
    sprintf(cmd, "cp %s/index.js wnpkg-build/source/index.js", argv[1]);

    if (system(cmd) == 0) {
      YELLOW_PRINT("[Log]: added app icon in source folder.\n");
    } else {
      printf(
          "[Error]:  Unable to move icon file in target folder, file may not "
          "exist.\n");
      return 1;
    }

    // Write resource file (.rc) for the icon
    FILE* write_icon = fopen("wnpkg-build\\source\\icon.rc", "w");
    if (write_icon == NULL) {
      RED_PRINT("[Error]:  Writing icon file config in wnpkg-build failed.\n");
      fclose(write_icon);
      exit(1);
    }

    // Generate .rc file content
    char icon_data[] = "1 ICON \"wnpkg-build\\source\\";
    strcat(icon_data, icon);
    strcat(icon_data, "\"");

    fprintf(write_icon, "%s", icon_data);
    fclose(write_icon);

    // Compile icon.rc into icon.o
    RESET_CMD();
    snprintf(cmd, sizeof(cmd),
             "windres wnpkg-build\\source\\icon.rc -O coff -o "
             "wnpkg-build\\source\\icon.o" /*, app_name*/);
    if (system(cmd) == 0) {
      YELLOW_PRINT("[Log]: added icon config in source folder.\n");
    } else {
      RED_PRINT("[Error]:  Unable to add icon config file in target folder.\n");
      return 1;
    }
    use_icon = 1;
  }

  // Build executable using GCC (with or without icon)
  RESET_CMD();
  if (use_icon == 0) {
    snprintf(cmd, sizeof(cmd),
             "gcc wnpkg-build/source/app.c wnpkg-build/source/node.s -o "
             "wnpkg-build/%s",
             app_name);
  } else {
    snprintf(cmd, sizeof(cmd),
             "gcc wnpkg-build/source/app.c wnpkg-build/source/node.s "
             "wnpkg-build/source/icon.o -o "
             "wnpkg-build/%s",
             app_name);
  }

  if (system(cmd) == 0) {
    YELLOW_PRINT("[Log]: added application executable in build folder.\n");
  } else {
    RED_PRINT(
        "[Error]:  unable to add application executable in build folder.\n");
  }

  fclose(fname);

  GREEN_PRINT("[Success]: Project compiled successfully.\n");

  return 0;
}