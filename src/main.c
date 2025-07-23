#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wnpkg/color.h"
#include "wnpkg/file.h"
#include "wnpkg/plat.h"

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
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

  int is_exe = 0;
#ifdef WNPKG_WINDOWS
  is_exe = 1;
#endif

  // C program that runs embedded Node.js app
#ifdef WNPKG_WINDOWS
  char builder[2048] = "#include <stdlib.h>\n#include <stdio.h>\n\nint "
                       "main(){\n\tsystem(\"source\\\\node.exe "
                       "source\\\\index.js\");\n\treturn 0;\n}";
#else
  char builder[2048] =
      "#include <stdlib.h>\n#include <stdio.h>\n\nint "
      "main(){\n\tsystem(\"chmod +x source/node\");\n\tsystem(\"./source/node "
      "source/index.js\");\n\treturn 0;\n}";
#endif

  YELLOW_PRINT("[Log]: Making Builder folder...\n");

  // Remove old build folder
  if (wnpkg_rmdir("wnpkg-build") == 0)
  {
    YELLOW_PRINT(
        "[Log]: Old builder folder (wnpkg-build) removed successfully.\n");
  }

  // Create build folder
  if (wnpkg_mkdir("wnpkg-build") == 0)
  {
    YELLOW_PRINT("[Log]: Builder folder (wnpkg-build) created successfully.\n");
  }
  else
  {
    printf(
        "[Error]:  Builder folder (wnpkg-build) may already exist or failed to "
        "create.\n");
    return 1;
  }

  // Create source subfolder
  YELLOW_PRINT("[Log]: Making Source folder...\n");
  if (wnpkg_mkdir("wnpkg-build/source") == 0)
  {
    YELLOW_PRINT(
        "[Log]: Source folder (wnpkg-build/source) created successfully.\n");
  }
  else
  {
    printf(
        "[Error]:  Source folder (wnpkg-build) may already exist or failed to "
        "create.\n");
    return 1;
  }

  char cmd[2048];

#define RESET_CMD() memset(cmd, 0, sizeof(cmd))

  // Copy index.js into build folder
#ifdef WNPKG_WINDOWS
  sprintf(cmd, "copy %s\\index.js wnpkg-build\\source\\index.js", argv[1]);
  sprintf(config_path, "%s\\wnpkg_config", argv[1]);
#else
  sprintf(cmd, "cp %s/index.js wnpkg-build/source/index.js", argv[1]);
  sprintf(config_path, "%s/wnpkg_config", argv[1]);
#endif

  if (system(cmd) == 0)
  {
    YELLOW_PRINT("[Log]: added index.js in source folder.\n");
  }
  else
  {
    printf(
        "[Error]:  Unable to move index.js file in target folder, file may not "
        "exist.\n");
    return 1;
  }

  // Copy package.json into build folder
#ifdef WNPKG_WINDOWS
  sprintf(cmd, "copy %s\\package.json wnpkg-build\\source\\package.json",
          argv[1]);
#else
  sprintf(cmd, "cp %s/package.json wnpkg-build/source/package.json", argv[1]);
#endif
  if (system(cmd) == 0)
  {
    YELLOW_PRINT("[Log]: added package.json in source folder.\n");
  }
  else
  {
    printf(
        "[Error]:  Unable to move package.json file in target folder, file may "
        "not exist.\n");
    return 1;
  }

  // reset cmd
  RESET_CMD();

  // Copy node_modules into build folder
  char folder[1024];
#ifdef WNPKG_WINDOWS
  snprintf(folder, sizeof(folder), "%s\\node_modules", argv[1]);
  if (wnpkg_havedir(folder))
  {
    sprintf(
        cmd,
        "xcopy /E /I /Q /Y %s wnpkg-build\\source\\node_modules",
        folder);
  }
#else
  if (wnpkg_havedir(folder))
  {
    snprintf(folder, sizeof(folder), "%s/node_modules", argv[1]);
    sprintf(cmd, "cp -rf %s/node_modules wnpkg-build/source/node_modules", argv[1]);
  }
#endif

  if (system(cmd) == 0)
  {
    YELLOW_PRINT("[Log]: added node modules in source folder.\n");
  }
  else
  {
    printf(
        "[Error]:  Unable to move node modules file in target folder, file may "
        "not exist.\n");
    return 1;
  }

  // Read app name and icon from configuration file
  FILE *fname = fopen(config_path, "r");
  if (fname == NULL)
  {
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
  if (strcmp(icon, "*") == 0)
  {
    YELLOW_PRINT("[Log]: Default icon defined successfully.\n");
  }
  else
  {
    // Copy user-defined icon into source folder
    RESET_CMD();
#ifdef WNPKG_WINDOWS
    sprintf(cmd, "copy %s\\%s wnpkg-build\\source\\%s", argv[1], icon, icon);
#else
    sprintf(cmd, "cp %s/index.js wnpkg-build/source/index.js", argv[1]);
#endif
    if (system(cmd) == 0)
    {
      YELLOW_PRINT("[Log]: added app icon in source folder.\n");
    }
    else
    {
      printf(
          "[Error]:  Unable to move icon file in target folder, file may not "
          "exist.\n");
      return 1;
    }

    // Write resource file (.rc) for the icon
    FILE *write_icon = fopen("wnpkg-build\\source\\icon.rc", "w");
    if (write_icon == NULL)
    {
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
    if (system(cmd) == 0)
    {
      YELLOW_PRINT("[Log]: added icon config in source folder.\n");
    }
    else
    {
      RED_PRINT("[Error]:  Unable to add icon config file in target folder.\n");
      return 1;
    }
    use_icon = 1;
  }

  // Write app.c source file with embedded launcher
#ifdef WNPKG_WINDOWS
  FILE *writeApp = fopen("wnpkg-build\\source\\app.c", "w");
#else
  FILE *writeApp = fopen("wnpkg-build/source/app.c", "w");
#endif
  if (writeApp == NULL)
  {
    RED_PRINT("[Error]:  Writing app in wnpkg-build failed.\n");
    fclose(writeApp);
    exit(1);
  }

  fprintf(writeApp, "%s", builder);
  fclose(writeApp);
  YELLOW_PRINT("[Log]: Application created successfully.\n");

  // Build executable using GCC (with or without icon)
  RESET_CMD();
  if (use_icon == 0)
  {
#ifdef WNPKG_WINDOWS
    snprintf(cmd, sizeof(cmd),
             "gcc wnpkg-build/source/app.c -o wnpkg-build/%s.exe "
             "-Wl,--subsystem,console",
             app_name);
#else
    snprintf(cmd, sizeof(cmd), "gcc wnpkg-build/source/app.c -o wnpkg-build/%s",
             app_name);
#endif
  }
  else
  {
#ifdef WNPKG_WINDOWS
    snprintf(cmd, sizeof(cmd),
             "gcc wnpkg-build/source/app.c wnpkg-build/source/icon.o -mwindows "
             "-o wnpkg-build/%s.exe -Wl,--subsystem,console",
             app_name);
#else
    snprintf(cmd, sizeof(cmd),
             "gcc wnpkg-build/source/app.c wnpkg-build/source/icon.o -o "
             "wnpkg-build/%s",
             app_name);
#endif
  }

  if (system(cmd) == 0)
  {
    YELLOW_PRINT("[Log]: added application executable in build folder.\n");
  }
  else
  {
    RED_PRINT(
        "[Error]:  unable to add application executable in build folder.\n");
  }

  // Copy node.exe into the source folder
  RESET_CMD();
#ifdef WNPKG_WINDOWS
  sprintf(cmd, "copy \"\\Program Files\\nodejs\\node.exe\" "
               "wnpkg-build\\source\\node.exe");
#else
  char npath[1024];
  FILE *fp = popen("which node", "r");
  if(fp == NULL) {
    printf("Error : failed to get node bin.");
    exit(1);
  }

  if(fgets(npath, sizeof(npath), fp) == NULL) {
    printf("Error : node-js not installed, install it.");
    pclose(fp);
    exit(1);
  }
  pclose(fp);
  npath[strcspn(npath, "\n")] = 0;

  sprintf(cmd, "cp %s wnpkg-build/source/node && chmod +x wnpkg-build/source/node", npath);
#endif

  if (system(cmd) == 0)
  {
    YELLOW_PRINT("[Log]: added %s in source folder.\n",
                 (is_exe) ? "node.exe" : "node");
  }
  else
  {
    RED_PRINT("[Error]:  Unable to move %s file in target folder, file may not "
              "exist.\n",
              (is_exe) ? "node.exe" : "node");
    return 1;
  }

  fclose(fname);

  GREEN_PRINT("[Success]: Project compiled successfully.\n");

  return 0;
}