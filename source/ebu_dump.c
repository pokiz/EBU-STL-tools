#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "ebu.h"
#include "string_utils.h"

int dump_line(T_EXPORT_FORMAT exportFormat, int last, char* opt, char* name, char* format, ...) {
	if (opt != NULL && strcmp(opt, name) != 0) {
		return 0;
	}

	va_list args;
	va_start(args, format);

	char  formattedValue[80];
	char  *trimmedValue;

	vsprintf(formattedValue, format, args);
	trimmedValue = trimString(formattedValue);

	if (opt != NULL && strcmp(opt, name) == 0) {
		printf("%s\n", trimmedValue);
	} else {
		if (exportFormat == RAW) {
			printf("%s: %s\n", name, trimmedValue);
		} else if (exportFormat == JSON) {
			printf("\"%s\": \"%s\"", name, trimmedValue);
			if (last == 0) printf(",");
		}
	}
	free(trimmedValue);
	va_end(args);
	return 1;
}

void dumpTTI(T_EXPORT_FORMAT exportFormat, int last, struct EBU_TTI* tti){
	dump_line(exportFormat, 0, NULL, "SGN", "%hX", tti->SGN);
	dump_line(exportFormat, 0, NULL, "SN", "%02hX%02hX", tti->SN[1], tti->SN[0]);
	dump_line(exportFormat, 0, NULL, "EBN", "%hX", tti->EBN);
	dump_line(exportFormat, 0, NULL, "CS", "%hX", tti->CS);

	dump_line(exportFormat, 0, NULL, "TCI", "%02d:%02d:%02d:%02d", tti->TCI.hours,tti->TCI.minutes,tti->TCI.seconds,tti->TCI.frames);
	dump_line(exportFormat, 0, NULL, "TCO", "%02d:%02d:%02d:%02d", tti->TCO.hours,tti->TCO.minutes,tti->TCO.seconds,tti->TCO.frames);
	dump_line(exportFormat, 0, NULL, "VP", "%hX", tti->VP);
	dump_line(exportFormat, 0, NULL, "JC", "%hX", tti->JC);
	dump_line(exportFormat, 0, NULL, "CF", "%hX", tti->CF);

	int i = 0;
	int j = 0;
	for(i = 0; i < 112; i++){
		if(tti->TF[i] == 0X1E){
			tti->TF[i] = 0x8A;
		}
		if(tti->TF[i] >= 0x20 && tti->TF[i] < 0x80){
			j++;
		}
		if(tti->TF[i] == 0x8A || tti->TF[i] == 0x8F){
			dump_line(exportFormat, 0, NULL, "Char Count", "%d\t%s", tti->TF);
			j = 0;
			if(tti->TF[i] == 0x8F){
				break;
			}
		}

	}
	dump_line(exportFormat, last, NULL, "TF", "%.112s", tti->TF);
}

void dumpGSI(T_EXPORT_FORMAT format, int full, char *option, struct EBU* ebu) {
	dump_line(format, 0, option, "CPN", "%.3s",ebu->gsi.CPN);
	dump_line(format, 0, option, "DFC", "%.8s",ebu->gsi.DFC);
	dump_line(format, 0, option, "DSC", "%c",ebu->gsi.DSC);
	dump_line(format, 0, option, "CCT", "%.2s",ebu->gsi.CCT);
	dump_line(format, 0, option, "LC",  "%.2s",ebu->gsi.LC);
	dump_line(format, 0, option, "OPT", "%.32s",ebu->gsi.OPT);
	dump_line(format, 0, option, "OET", "%.32s",ebu->gsi.OET);
	dump_line(format, 0, option, "TPT", "%.32s",ebu->gsi.TPT);
	dump_line(format, 0, option, "TET", "%.32s",ebu->gsi.TET);
	dump_line(format, 0, option, "TN",  "%.32s",ebu->gsi.TN);
	dump_line(format, 0, option, "TCD", "%.32s",ebu->gsi.TCD);
	dump_line(format, 0, option, "SLR", "%.16s",ebu->gsi.SLR);
	dump_line(format, 0, option, "CD",  "%.6s",ebu->gsi.CD);
	dump_line(format, 0, option, "RD",  "%.6s",ebu->gsi.RD);
	dump_line(format, 0, option, "RN",  "%.2s",ebu->gsi.RN);
	dump_line(format, 0, option, "TNB", "%.5s",ebu->gsi.TNB);
	dump_line(format, 0, option, "TNS", "%.5s",ebu->gsi.TNS);
	dump_line(format, 0, option, "TNG", "%.3s",ebu->gsi.TNG);
	dump_line(format, 0, option, "MNC", "%.2s",ebu->gsi.MNC);
	dump_line(format, 0, option, "MNR", "%.2s",ebu->gsi.MNR);
	dump_line(format, 0, option, "TCS", "%hX",ebu->gsi.TCS);

	struct EBU_TC *tc = charToTC(ebu->gsi.TCP);
	dump_line(format, 0, option, "TCP", "%02hd:%02hd:%02hd:%02hd", tc->hours,tc->minutes,tc->seconds,tc->frames);
	free(tc);
	tc = charToTC(ebu->gsi.TCF);
	dump_line(format, 0, option, "TCF", "%02hd:%02hd:%02hd:%02hd", tc->hours,tc->minutes,tc->seconds,tc->frames);
	free(tc);

	dump_line(format, 0, option, "TND", "%hX",ebu->gsi.TND);
	dump_line(format, 0, option, "DSN", "%hX",ebu->gsi.DSN);
	dump_line(format, 0, option, "CO",  "%.3s",ebu->gsi.CO);
	dump_line(format, 0, option, "PUB", "%.32s",ebu->gsi.PUB);
	dump_line(format, 0, option, "EN",  "%.32s",ebu->gsi.EN);
	dump_line(format, (int)(!full), option, "ECD", "%.32s",ebu->gsi.ECD);
}

void dump(T_EXPORT_FORMAT format, char *option, struct EBU* ebu, int full) {
	int i = 0;

	if (!option && format == JSON) {
		printf("{");
	}

	dumpGSI(format, full, option, ebu);

	if(full == 1){
		char TNB[6];
		strncpy(TNB,ebu->gsi.TNB,5);
		TNB[5] = '\0';
		int nTNB = atoi(TNB);
		printf("%d\n",nTNB);
		for(i = 0; i < nTNB; i++){
			dumpTTI(format, (int)(i+1 == nTNB), &(ebu->tti[i]));
		}
	}

	if (!option && format == JSON) {
		printf("}");
	}
}

int main(int argc, const char** argv) {
	char * output = NULL;
	char * option = NULL;
	T_EXPORT_FORMAT format = RAW;

	int full = 0;
	int i = 0;
	for (i = 1; i < argc; ++i)
	{
		if (!strcmp(argv[i],"-f")) {
			full = 1;
		} else if (strstr(argv[i], "--option=") != NULL) {
			option = strtok(argv[i], "--option=");
		} else if (strstr(argv[i], "--json") != NULL) {
			format = JSON;
		} else if (strstr(argv[i], "--raw") != NULL) {
			format = RAW;
		} else {
			output = (char *) argv[i];
		}
	}

	if (output == NULL) {
		if (output == NULL)
			printf("no output set\n");
		printf("Usage: %s [-f] [--option={option}] [--raw] [--json] input.stl\n",argv[0]);
		return 0;
	}

	FILE* source = fopen(output,"r");
	if(source == NULL){
		printf("Error: Source not loaded\n");
		fclose(source);
		return 1;
	}

	if (full == 1) {
		option = NULL;
	}

	struct EBU* ebu = parseEBU(source);
	fclose(source);

	isBelleNuit(ebu);

	dump(format, option, ebu, full);

	free(ebu);

	return 0;
}
