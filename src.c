#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define BASE 2.0
#define SHELL_TOK_BUFSIZE 64; 
#define SHELL_TOK_DELIM  " \t\r\n\a"

/* error handling */ 
void assert(int condition, const char* message) {
  if (!condition) {
    printf("%s\n", message);
    exit(1);
  }
}

char *reverse_str(char *str) {
	int i = strlen(str); 
	int j = 0; 
	char *rev_str = malloc(sizeof(rev_str)); 
	while (i > 0) {
		i--; 
		rev_str[j] = str[i]; 
		j++; 
	}
	return rev_str;  
}

char *shell_read_line() {
	
	int position = 0;
	char *buffer = malloc(sizeof(buffer)); 
	int c; 

	assert(buffer != NULL, "Instruction size too large"); 


	while (1) {  /* reading standard input */ 

		c = getchar(); 

		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer; 
		} else {
			buffer[position] = c;
		}
		position++; 
	}

	return buffer; 
}

char **shell_split_line(char *line) {
	
	int position = 0;
	int bufsize = SHELL_TOK_BUFSIZE; 

	char **tokens = malloc(sizeof(char*) * bufsize);
	char *token;

	assert(tokens != NULL, "Memory allocation error"); 

	token = strtok(line, SHELL_TOK_DELIM);  /* splits line and returns pointer to first "token" */ 
	while (token != NULL) {
		tokens[position] = token; 
		position++; 

		if (position >= bufsize) {
			bufsize += SHELL_TOK_BUFSIZE;
			tokens = realloc(tokens, bufsize);
			assert(tokens != NULL, "Memory Allocation Error");
		}
		token = strtok(NULL, SHELL_TOK_DELIM);  /* gets next token */ 
	}
	tokens[position] = NULL; 
	return tokens; 
}

char **fetch_instructions() {
	char *line = shell_read_line(); 
	char **args = shell_split_line(line); 
	return args; 
}

/* converts binary to decimal (unsigned) */ 
int convert_binary(char *value) {
	int num; 
	int i = strlen(value); 
	int result = 0; 
	float exponent = 0; 

	for (i = strlen(value) - 1; i >= 0; i--) {
		num = value[i] - '0';  /* converting char to int */ 
		assert(num == 0 || num == 1, "Invalid Binary Number");  /* error if bit is not binary */ 
		if (num == 1) {
			result = result + powf(BASE, exponent);
		}
		exponent++; 
	}
	return result; 
}

/* converts decimal value to binary */ 
char *convert_decimal(char *value) {
	char *reverse_binary = malloc(sizeof(reverse_binary));
	char *binary_value;  
	int i = 0; 
	int dividend = atoi(value); 

	while (dividend >= 1) {
		if (dividend % 2 == 0) {
			reverse_binary[i] = '0';
		}  
		else {
			reverse_binary[i] = '1'; 
		}
		dividend = dividend / 2; 
		i++; 
	}

	binary_value = reverse_str(reverse_binary); 
	free(reverse_binary); 

	return binary_value; 
}

char *add_binary(char *value1, char *value2) {
	char *reverse_binary = malloc(sizeof(reverse_binary)); 
	char *binary_result; 
	char *reverse_value1 = reverse_str(value1); 
	char *reverse_value2 = reverse_str(value2);  
	int int_value1, int_value2;  
	int i;
	int bit; 
	int carry_value = 0; 

	/* ensuring binary values have same length */ 
	if (strlen(reverse_value1) != strlen(reverse_value2)) {
		i = strlen(reverse_value1);
		while (strlen(reverse_value1) < strlen(reverse_value2)) {
			reverse_value1[i] = '0';
			i++; 
		}
		i = strlen(reverse_value2); 
		while (strlen(reverse_value2) < strlen(reverse_value1)) {
			reverse_value2[i] = '0'; 
			i++; 
		}
	}

	for (i = 0; i < strlen(reverse_value1); i++) {
		int_value1 = reverse_value1[i] - '0';  
		int_value2 = reverse_value2[i] - '0';
		bit = int_value1 + int_value2 + carry_value; 
		carry_value = 0; 
		if (bit == 2) {
			bit = 0; 
			carry_value = 1; 
		}
		if (bit == 3) {
			bit = 1;
			carry_value = 1; 
		}

		reverse_binary[i] = bit + '0'; 
	}
	if (carry_value == 1) {  /* accounting for bit overflow */ 
		reverse_binary[i] = '1'; 
	} 

	free(reverse_value1); 
	free(reverse_value2); 

	binary_result = reverse_str(reverse_binary); 
	free(reverse_binary); 

	return binary_result; 

}

/* flips 0s to 1s and 1s to 0s */ 
char *flip_binary(char *value) {
	int i = 0; 
	char *flipped_value = malloc(sizeof(flipped_value)); 

	for (i = 0; i < strlen(value); i++) {
		if (value[i] == '0') {
			flipped_value[i] = '1'; 
		}
		else {
			flipped_value[i] = '0'; 
		}
	}

	return flipped_value; 
}

/* converts to negative binary value */ 
char *twos_complement(char *value) {
	char *negative_value; 
	char *flipped_value; 
	char *one = "1"; 
	int i; 

	flipped_value = flip_binary(value);

	negative_value = add_binary(flipped_value, one); 

	return negative_value; 

}

char *convert_decimal_signed(char *value) {
	int i = 0;
	char *one = "1"; 
	char *result = malloc(sizeof(result)); 
	char *flipped_value; 

	/* if number if positive: convert like normal */ 
	if (value[i] != '-') {
		result = convert_decimal(value); 
	}

	value = value + 1; 
	value = convert_decimal(value); 
	result = twos_complement(value); 

	return result; 
}

int convert_binary_signed(char *value) {
	int i = 0; 
	char *temp_result; 
	char *binary_result; 
	char *negative_one = malloc(sizeof(negative_one)); 
	int result, dec_result, neg_result; 

	/* if number if positive */ 
	if (value[i] == '0') {
		result = convert_binary(value); 
		return result; 
	}

	/* negative 1 - has same length as binary value to be converted (for easy addition) */ 
	for (i = 0; i < strlen(value); i++) {
		negative_one[i] = '1'; 
	}

	temp_result = add_binary(value, negative_one); 
	binary_result = flip_binary(temp_result); 
	dec_result = convert_binary(binary_result); 
	neg_result = dec_result * (-1); 

	free(temp_result);
	free(binary_result); 
	free(negative_one); 

	return neg_result; 	

}

char *subtract_binary(char *value1, char *value2) {
	char *result; 
	char *reverse_value2;
	int i = strlen(value2); 

	value2 = twos_complement(value2); 

	/* padding negative number with 1s */ 
	reverse_value2 = reverse_str(value2); 
	while (strlen(reverse_value2) < strlen(value1)) {
		reverse_value2[i] = '1';
		i++;  
	}

	value2 = reverse_str(reverse_value2); 
	result = add_binary(value1, value2); 

	free(value2);
	free(reverse_value2); 

	return result; 
}

void evaluate(char **instructions) {  
	char *opcode = instructions[0]; 
	char *value = instructions[1]; 

	union Result {
		int decimal; 
		char *binary; 
	} result; 

	if (!strcmp(opcode, "convert_binary")) {
		result.decimal = convert_binary(value); 
		printf("Decimal Result: %d\n", result.decimal); 
	}
	else if (!strcmp(opcode, "convert_decimal")) {
		result.binary = convert_decimal(value);
		printf("Binary Result: %s\n", result.binary); 
	} 
	else if (!strcmp(opcode, "convert_negative")) {
		result.binary = twos_complement(value); 
		printf("Binary Result: %s\n", result.binary); 
		result.decimal = convert_binary_signed(result.binary);
		printf("Decimal Result: %d\n", result.decimal); 
	}
	else if (!strcmp(opcode, "convert_binary_signed")) {
		result.decimal = convert_binary_signed(value); 
		printf("Decimal Result: %d\n", result.decimal); 
	}
	else if (!strcmp(opcode, "convert_decimal_signed")) {
		result.binary = convert_decimal_signed(value); 
		printf("Binary Result: %s\n", result.binary); 
	}
	else if (!strcmp(opcode, "add")) {
		char *value2 = instructions[2]; 
		result.binary = add_binary(value, value2);
		printf("Binary Result: %s\n", result.binary); 
		result.decimal = convert_binary(result.binary); 
		printf("Decimal Result: %d\n", result.decimal); 
	}
	else if (!strcmp(opcode, "subtract")) {
		char *value2 = instructions[2]; 
		result.binary = subtract_binary(value, value2);
		printf("Binary Result: %s\n", result.binary); 
		result.decimal = convert_binary_signed(result.binary);
		printf("Decimal Result: %d\n", result.decimal); 
	}
	else {
		printf("Invalid Instruction Given!\n");
		exit(1); 
	}

}

void bit_calc() {
	bool running = true; 

	while (running) {
		char **instructions = fetch_instructions(); 
		evaluate(instructions); 
	}
}