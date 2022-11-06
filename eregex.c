#include "parsepages.h"

size_t BLOCK_SIZE = 8;
size_t ALLOC_SIZE = 4;

struct content{
	char* str;
	uint32_t len;
	uint32_t offset;
	uint32_t sp;
	uint32_t ep;
};


content** findAll(char* input, char* expression){
	
	uint32_t retSize = ALLOC_SIZE;
	uint32_t offset = 0;
	content** ret = malloc(retSize * sizeof(content*));
	
	for(unsigned int i = 0; i < retSize; i++){
		ret[i] = malloc(sizeof(content));
	}

	uint32_t sz = 0;
	content* retVal;
	while((retVal = findMatch(input, expression)) != NULL){
		offset += strlen(input) - strlen(input + retVal->ep);
		input += retVal->ep;
		retVal->offset = offset;
		memcpy(ret[sz], retVal, sizeof(content));
		free(retVal);
		sz++;
		if(sz == retSize){
			retSize += ALLOC_SIZE;
			ret = realloc(ret, retSize * sizeof(content*));
			for(unsigned int i = sz; i < retSize; i++){
				ret[i] = malloc(sizeof(content));
			}
		}
	}

	free(ret[sz]);	
	ret[sz] = NULL;
	for(uint32_t i = sz+1; i < retSize; i++){
		free(ret[i]);
	}

	return ret;	
}



content* findMatch(char* input, char* expression){
	
	regex_t regexxx;
	regmatch_t rmatch[2];
	content* ret = malloc(sizeof(content));
	
	if(regcomp(&regexxx, expression, REG_EXTENDED)){
		free(ret);
		return NULL;
	}

	if(regexec(&regexxx, input, 2, rmatch, 0)){
		free(ret);
		regfree(&regexxx);
		return NULL;
	}


	ret->len = rmatch[1].rm_eo - rmatch[1].rm_so;
	ret->str = malloc(ret->len + 1);
	ret->str[ret->len] = 0;
	memcpy(ret->str, &input[rmatch[1].rm_so], ret->len);
	ret->sp = rmatch[1].rm_so;
	ret->ep = rmatch[1].rm_eo;


	regfree(&regexxx);

	return ret;
}


char* replace(char* input, uint32_t dest, uint32_t len, char* replacement){

	if(len == 0) {
		puts("Length can't be 0!");
		return NULL;
	}

	char* ret;
	uint32_t inpLen = strlen(input);
	uint32_t repLen = strlen(replacement);
	uint32_t newLen = inpLen + repLen;

	ret = malloc(newLen + repLen + 1);
	memcpy(ret, input, newLen + 1);

	if(repLen > len){
		memcpy(&ret[dest], replacement, repLen);
		memcpy(&ret[dest+repLen], &input[dest+len], inpLen - dest - len);
	} else if (repLen < len){
		memcpy(&ret[dest], replacement, repLen);
		memcpy(&ret[dest+repLen], &input[dest+repLen+(len-repLen)], inpLen - dest - len);		
	} else {
		memcpy(ret, input, inpLen);
		memcpy(&ret[dest], replacement, repLen);
	}
	
	ret[newLen] = '\0';
	
	return ret;
}

char* replaceMatch(char* input, char* expression, char* replacement){

	content* match = findMatch(input, expression);
	char* ret = replace(input, match->sp, match->len, replacement);
	free(match->str);
	free(match);
	return ret;

}

char* replaceAll(char* input, char* expression, char* replacement){
	
	uint32_t repLen = strlen(replacement);
	uint32_t inpLen = strlen(input);
	uint32_t totalAlloc = inpLen + 1;
	char* ret = malloc(totalAlloc);
	uint32_t offset = 0;
	content* temp;	
	
	while((temp = findMatch(input, expression)) != NULL){
		int32_t diff = repLen - temp->len;
		totalAlloc += diff;
		ret = realloc(ret, totalAlloc + diff);
		char* replaced = replace(input, temp->sp, temp->len, replacement);
		memmove(input, &input[temp->ep], inpLen - offset);
		memcpy(&ret[offset], replaced, temp->sp + repLen);
		offset += temp->sp + repLen;
		free(replaced);
		free(temp->str);
		free(temp);	
	}
	ret = realloc(ret, totalAlloc + strlen(input));
	memcpy(&ret[offset], input, strlen(input));
	ret[offset+strlen(input)] = '\0';

	return ret;

}

char* removeAll(char* input, char* expression){

	content** val = findAll(input, expression);
	uint32_t inpLen = strlen(input);
	char* outp = malloc(inpLen);
	uint32_t offset = 0;
	uint32_t totalL = 0;
	uint32_t i = 0;
	char* temp;

	for(; val[i] != NULL; i++){
		uint32_t len = val[i]->offset - val[i]->len - offset;
		temp = malloc(len);	
		memcpy(temp, &input[offset], len);
		memcpy(outp + totalL, temp, len);
		totalL += len;
		offset = val[i]->offset;	
		free(temp);
	}

	memcpy(outp + totalL, &input[val[i-1]->offset], inpLen - val[i-1]->offset);
       	totalL += inpLen - val[i-1]->offset;	
	outp[totalL] = '\0';
	outp = realloc(outp, totalL + 1);

	freeAll(val);

	return outp;

}

void freeAll(content** dest){

	for(uint32_t i = 0; *(dest+i) != NULL; i++){
		free(dest[i]->str);
		free(dest[i]);
	}
	free(dest);
}

void printMatches(content** dest){
	uint32_t i = 0;
	for(; *(dest+i) != NULL; i++){
		printf("Match: %s\tLen: %d\n", (*(dest+i))->str, (*(dest+i))->len);
	}
	printf("Total matches: %d\n", i);
}

