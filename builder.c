#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>

int main(int argc, char *argv[]) {
	char path[1024];
	char icon[1024];
	char appname[1024];
	char confpath[1024];
	int letter = 0;
	char c;
	int use_icon = 0;
	
	// C program that runs embedded Node.js app
	char builder[2048] = "#include <stdlib.h>\n#include <stdio.h>\n\nint main(){\n\tsystem(\"source\\\\node.exe source\\\\index.js\");\n\treturn 0;\n}";

	printf("Making Builder folder...\n");

	// Remove old build folder
	if(rmdir("build-last") == 0) {
		printf("Log: Old builder folder (build-last) removed successfully.\n");
	}

	// Create build folder
	if(_mkdir("build-last") == 0) {
		printf("Log: Builder folder (build-last) created successfully.\n");
	} else {
		printf("Error: Builder folder (build-last) may already exist or failed to create.\n");
		return 1;
	}
	
	// Create source subfolder
	printf("Making Source folder...\n");
	if(_mkdir("build-last/source") == 0) {
		printf("Log: Source folder (build-last/source) created successfully.\n");
	} else {
		printf("Error: Source folder (build-last) may already exist or failed to create.\n");
		return 1;
	}
	
	char cmd[2048];
	
	// Copy index.js into build folder
	sprintf(cmd, "copy %s\\index.js build-last\\source\\index.js", argv[1]);
	sprintf(confpath, "%s\\lbuilder", argv[1]);

	if(system(cmd) == 0) {
		printf("Log: added index.js in source folder.\n");
	}else{
		printf("Error: Unable to move index.js file in target folder, file may not exist.\n");
		return 1;
	}
	
	memset(cmd, 0, sizeof(cmd));
	
	// Copy package.json into build folder
	sprintf(cmd, "copy %s\\package.json build-last\\source\\package.json", argv[1]);

	if(system(cmd) == 0) {
		printf("Log: added package.json in source folder.\n");
	}else{
		printf("Error: Unable to move package.json file in target folder, file may not exist.\n");
		return 1;
	}
	
	memset(cmd, 0, sizeof(cmd));
	
	// Copy node_modules into build folder
	sprintf(cmd, "xcopy /E /I /Q /Y %s\\node_modules build-last\\source\\node_modules", argv[1]);

	if(system(cmd) == 0) {
		printf("Log: added node modules in source folder.\n");
	}else{
		printf("Error: Unable to move node modules file in target folder, file may not exist.\n");
		return 1;
	}
	
	// Read app name and icon from configuration file
	FILE *fname = fopen(confpath, "r");
	if(fname == NULL) {
		printf("Error : Config file (lbuilder) not found.\n");
		fclose(fname);
		exit(1);
	}
	
	fgets(appname, sizeof(appname), fname);
	appname[strcspn(appname, "\r\n")] = 0;
	
	// Parse app name until delimiter
	letter = 0;
	while(appname[letter] != ';') letter++;
	appname[letter] = '\0';
	
	fgets(icon, sizeof(icon), fname);
	icon[strcspn(icon, "\r\n")] = 0;
	
	// Parse icon name until delimiter
	letter = 0;
	while(icon[letter] != ';') letter++;
	icon[letter] = '\0';
	
	// Check if user wants default icon
	if(strcmp(icon, "*") == 0) {
		printf("Log: Default icon defined successfully.\n");
	} else {
		// Copy user-defined icon into source folder
		memset(cmd, 0, sizeof(cmd));
		sprintf(cmd, "copy %s\\%s build-last\\source\\%s", argv[1], icon, icon);

		if(system(cmd) == 0) {
			printf("Log: added app icon in source folder.\n");
		}else{
			printf("Error: Unable to move icon file in target folder, file may not exist.\n");
			return 1;
		}
		
		// Write resource file (.rc) for the icon
		FILE *writeIcon = fopen("build-last\\source\\icon.rc", "w");
		if(writeIcon == NULL) {
			printf("Error: Writing icon file config in build-last failed.\n");
			fclose(writeIcon);
			exit(1);
		}
		
		// Generate .rc file content
		char icon_data[] = "1 ICON \"build-last\\source\\";
		strcat(icon_data, icon);
		strcat(icon_data, "\"");
		
		fprintf(writeIcon, icon_data);
		fclose(writeIcon);
		
		// Compile icon.rc into icon.o
		memset(cmd, 0, sizeof(cmd));
		snprintf(cmd, sizeof(cmd), "windres build-last\\source\\icon.rc -O coff -o build-last\\source\\icon.o", appname);
		if(system(cmd) == 0) {
			printf("Log: added icon config in source folder.\n");
		}else{
			printf("Error: Unable to add icon config file in target folder.\n");
			return 1;
		}
		use_icon = 1;
	}
	
	// Write app.c source file with embedded launcher
	FILE *writeApp = fopen("build-last\\source\\app.c", "w");
	if(writeApp == NULL) {
		printf("Error: Writing app in build-last failed.\n");
		fclose(writeApp);
		exit(1);
	}
	
	fprintf(writeApp, builder);
	fclose(writeApp);
	printf("Log: Application created successfully.\n");
	
	// Build executable using GCC (with or without icon)
	memset(cmd, 0, sizeof(cmd));
	if(use_icon == 0) {
		snprintf(cmd, sizeof(cmd), "gcc build-last/source/app.c -o build-last/%s.exe -Wl,--subsystem,console", appname);
	} else {
		snprintf(cmd, sizeof(cmd), "gcc build-last/source/app.c build-last/source/icon.o -mwindows -o build-last/%s.exe -Wl,--subsystem,console", appname);
	}
	
	if(system(cmd) == 0) {
		printf("Log: added application executable in build folder.\n");
	} else {
		printf("Error: unable to add application executable in build folder.\n");
	}
	
	// Copy node.exe into the source folder
	memset(cmd, 0, sizeof(cmd));
	sprintf(cmd, "copy \"\\Program Files\\nodejs\\node.exe\" build-last\\source\\node.exe");

	if(system(cmd) == 0) {
		printf("Log: added node.exe in source folder.\n");
	}else{
		printf("Error: Unable to move node.exe file in target folder, file may not exist.\n");
		return 1;
	}
	
	fclose(fname);
	
	printf("\n\n[Done]\n");
	
	return 0;
}