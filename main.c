#include "stdio.h"
#include "conio.h"

typedef unsigned char U8;
typedef unsigned int U32;

U8 isSpace(U8 c)
{
	return (c == ' ') ? 1 : 0;
}

U8 isDigit(U8 c)
{
	if ((c >= '0') && (c <= '9')) {
		return 1;
	} 

	return 0;
}

#define DIGIT_BUFFER_SIZE 16

U8 digit_buffer[DIGIT_BUFFER_SIZE];
U8 digit_buffer_index = 0;

void push_digit_in(U8 c)
{
	if (!isDigit(c)) {
		return;
	}

	if (digit_buffer_index < (DIGIT_BUFFER_SIZE - 1)) {
		digit_buffer[digit_buffer_index++] = c;
	}
}

U8 current_operator_level = 0;
U8 negative_sign = 0;


#define OPERATOR_BUFFER_SIZE 128
U8 operator_buffer[OPERATOR_BUFFER_SIZE];
U8 operator_buffer_index = 0;


U8 operator_level(U8 c)
{
	U8 level = 0;

	switch (c) {
	case '+':
	case '-':
		level = 2;
		break;

	case '*':
	case '/':
		level = 1;
		break;

	case '(':
		level = 0;
		break;

	case ')':
		level = 127;
		break;

	case '\r':
		level = 128;
		break;

	default:
		level = 0xff;

	}

	return level;
}

void push_operator_in(U8 c)
{
	U8 valid_operator = 0;
	U8 level;
	
	if ((current_operator_level == 0) && (c == '-')) {
		negative_sign = 1;
	}
	else {

		level = operator_level(c);

		if (level != 0xff) {
			valid_operator = 1;
			current_operator_level = level;
		}
		
	}

	if ((operator_buffer_index < OPERATOR_BUFFER_SIZE) && (valid_operator)) {
		operator_buffer[operator_buffer_index++] = c;
	}
}

int extract_number()
{
	int temp_val = 0;
	U8 index = 0;

	while (index < digit_buffer_index) {
		temp_val = temp_val * 10 + (digit_buffer[index] - '0');
		++index;
	}

	digit_buffer_index = 0;

	printf("\n=================> %d\n", temp_val);

	if (negative_sign) {
		temp_val = -temp_val;
	}

	negative_sign = 0;

	return temp_val;

}

int num_stack[256];
U8 num_stack_index = 0;

void push_num(int num)
{
	if (num_stack_index != 255) {
		num_stack[num_stack_index++] = num;
	}
}

int pop_num()
{
	if (!num_stack_index) {
		return 0;
	}

	return (num_stack[--num_stack_index]);
}

U8 is_valid_expression_char(U8 c)
{
	if (isDigit(c) || (c == '(') || (c == ')') || (c == '-') ||
		(c == '+') || (c == '*') || (c == '/') || (c == '\r')) {
		return 1;
	} 

	return 0;
}

U8 calc_process()
{
	U8 c, p;
	int a, b;

	if (operator_buffer_index < 2) {
		return 0;
	}

	c = operator_buffer[--operator_buffer_index];
		
	while (operator_buffer_index) {
		p = operator_buffer[operator_buffer_index - 1];

		if (operator_level(p) <= operator_level(c)) {
			switch (p) {
				case '+':
				case '-':
					if (num_stack_index < 2) {
						return 0xFF;
					}

					b = pop_num();
					a = pop_num();
					printf("\n====>  %d %c %d \n", a, p, b);
					if (p == '+') {
						push_num(a + b);
					} else {
						push_num(a - b);
					}

					--operator_buffer_index;
					break;

				case '*':
				case '/':
					if (num_stack_index < 2) {
						return 0xFF;
					}

					b = pop_num();
					a = pop_num();
					
					printf("\n====>  %d %c %d \n", a, p, b);

					if (p == '*') {
						push_num(a * b);
					}
					else {
						push_num(a / b);
					}

					--operator_buffer_index;
					break;

				case '(':
					printf("\n====> ((( \n");
					if (c == ')') {
						--operator_buffer_index;
					} else {
						operator_buffer[operator_buffer_index++] = c;
						
					}

					return 0;

				default:
					return 0xff;
			} // End of switch

		} else {
			operator_buffer[operator_buffer_index++] = c;
			break;
		}

	}
	
	return 0;

}


U8 input_buffer[256];
U8 input_buffer_index = 0;

void get_expression()
{
	U8 c;

	input_buffer_index = 0;
	num_stack_index = 0;
	digit_buffer_index = 0;
	current_operator_level = 0;
	negative_sign = 0;
    operator_buffer_index = 0;

	do {
		c = _getch();
		putchar(c);
		if ((c == 8) && (input_buffer_index)) {
			putchar(' ');
			putchar('\b');
			--input_buffer_index;
		} else if (input_buffer_index < 255) {
			input_buffer[input_buffer_index++] = c;
		}
	} while (c != '\r');
	
	input_buffer[input_buffer_index] = '\0';

}

void main()
{
	U8 c;
	U8 i;
	while (1) {
		printf("\nPlease input an expression to evaluate:\n");

		get_expression();

		for (i = 0; i < input_buffer_index; ++i) {

			c = input_buffer[i];
			if (is_valid_expression_char(c)) {
				push_digit_in(c);
				push_operator_in(c);

				if (!isDigit(c)) {
					if (digit_buffer_index) {
						push_num(extract_number());
					}

					if (calc_process()) {
						printf("error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
					}
				}


				
			}
		}

		printf("operator_buffer_index = %d\n", operator_buffer_index);
		if (operator_buffer_index && (operator_buffer[0] != '\r')) {
			printf("Expression error!\n");
		} else {
			printf("\n\n result = %d\n", num_stack[0]);
		}
	}

}

