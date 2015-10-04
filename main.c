
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/time.h>
#include <errno.h>
#include <psp2/types.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/sysmem.h>
#include <psp2/uvl.h>
#include "shaders.h"


	
	SceShaccCgSourceFile g_input;
		
	void *get_input(void) {
		return &g_input;
	}
	
	SceShaccCgCompileOutput *get_shader(int type) {
		
		int ret = 0;
		
		// Init the library
		/*ret = SceShaccCg_935CD196(0);
		printf("SceShaccCg_935CD196: 0x%08x\n", ret);*/

		// Register malloc and free function pointers
		//ret = SceShaccCg_6F01D573(malloc, free);
		//printf("SceShaccCg_6F01D573: 0x%08x\n", ret);
		
		g_input.fileName = "<built-in>";
		
		shader_first first = {0};
		SceShaccCgCompileOptions second = {0};
		
		/*ret = SceShaccCg_A8C2C1C8(first, 1);
		printf("SceShaccCg_A8C2C1C8: 0x%08x\n", ret);*/
		ret = sceShaccCgInitializeCompileOptions(second);
		printf("SceShaccCg_3B58AFA0: 0x%08x\n", ret);
		
		
		first.field_0 = (int)get_input;

		second.mainSourceFile = g_input.fileName;
		second.targetProfile =  type; // 0 -- vertex, 1 -- fragment
		second.entryFunctionName = "main";
		second.macroDefinitionCount = 1;
		const char *str = "SHADER_API_PSM";
		second.macroDefinitions = &str;
		second.useFx = 1;
		second.optimizationLevel = 3;
		second.useFastmath = 1;
		second.useFastint = 0;
		second.performanceWarnings = 0;
		second.warningLevel = 1;
		second.pedantic = 3;

		SceShaccCgCompileOutput *r = sceShaccCgCompile(&second, &first, 0);
		printf("SceShaccCg_66814F35: %p\n", r);
		return r;
	}

	
int main(){
	int shader_type;
	shader_type = SCE_SHACCCG_PROFILE_VP;
	
	FILE *input = fopen("cache0:/vertex2.shader", "rb");
	if (!input) {
		printf("%s: cannot open, errno %d\n", "cache0:/vertex.shader", errno);
		return -2;
	}
	fseek(input, 0, SEEK_END);
	g_input.size = ftell(input);
	fseek(input, 0, SEEK_SET);
	g_input.program = malloc(g_input.size);
	fread(g_input.program, g_input.size, 1, input);
	fclose(input);
	const SceShaccCgCompileOutput *r = get_shader(shader_type);
	if (!r) {
		printf("Unknown error\n");
		return -2;
	}

	// print infos/warnings/errors
	printf("diagnosticCount %lu\n", r->diagnosticCount);
	for (int i = 0; i < r->diagnosticCount; ++i) {
		const SceShaccCgDiagnosticMessage *e = &r->diagnostics[i];
		uint32_t x = 0, y = 0;
		if (e->location) {
			x = e->location->lineNumber;
			y = e->location->columnNumber;
		}
		switch (e->level) {
		case SCE_SHACCCG_DIAGNOSTIC_LEVEL_INFO:
			printf("INFO (%lu, %lu):  \n", x, y);
			break;
		case SCE_SHACCCG_DIAGNOSTIC_LEVEL_WARNING:
			printf("WARN (%lu, %lu):  \n", x, y);
			break;
		case SCE_SHACCCG_DIAGNOSTIC_LEVEL_ERROR:
			printf("ERROR (%lu, %lu):  \n", x, y);
			break;
		}
		//printf("%s\n", e->message);
	}
			
	if (r->programData) {
		FILE *output = fopen("cache0:/vertex4.out", "wb");
		if (!output) {
			printf("%s: cannot open for write, errno %d\n", "cache0:/vertex2.out", errno);
			return -2;
		}
		fwrite(r->programData, r->programSize, 1, output);
		fclose(output);
	} else {
		return -1;
	}
	printf("Shader compiled OK\n");
	return 0;

}
