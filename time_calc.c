/*
Bronson Schultz 11231230 bcs269
CMPT 214 Assignment 3
*/

#include <stdio.h>	// for printf, fprintf, puts
#include <stdlib.h> // for NULL, atol, malloc
#include <string.h> // for strlen, strcmp, strncpy, strtok
#include <ctype.h> // for isdigit
#include <regex.h> // for regex functions
#include <sys/types.h> // for regex functions
#include <assert.h> // for assert

#define MAX_SECONDS 3155760000 // this is the amount of seconds in a century

#define SECONDS_IN_YEAR 31557600 // used for converting the different amounts of
#define SECONDS_IN_MONTH 2629800 // time from one to another
#define SECONDS_IN_DAY 86400
#define SECONDS_IN_HOUR 3600
#define SECONDS_IN_MINUTE 60

typedef unsigned char bool; // using bool for ease of reading
typedef u_int32_t num_seconds_t;

// time spec struct for holding the number values of each time denominator
struct time_spec_t {
	u_int8_t year;
	u_int8_t month;
	u_int8_t day;
	u_int8_t hour;
	u_int8_t minute;
	u_int8_t second;
};


/*
function that looks at a string and determines whether it fits into the proper
format or not. This time spec string must follow the format of
n1y n2n n3d n4h n5m n6s - where all n are unsigned numbers and n1 < 100 n2 < 12
n3 < 31 n4 < 24 n5 < 60 n6 < 60. Also, the time spec must not be equal or
greater to 99y 11n 30d 10h 30m 0s. There can be any amount of white space
each field, but all fields must be present. Function returns 1 (TRUE) if the
string follows the format and restrictions, or prints an error and returns
0 (FALSE) otherwise.
*/
bool is_legal_time_spec(char *str_p){
	// initialize the regex variables
	regex_t regex1;
	regex_t regex2;

	// two ints to store the results of regex
	int ret1;
	int ret2;

	/*
	the first regular expression to check if the string is a valid time spec.
	Each n is within the proper bounds, in order for the string to be valid,
	it should match this pattern [0-99]y[0-11]n[0-30]d[0-23]h[0-59]m[0-59]s.
	Note: there can be any amount of whitespace between each number letter pair.
	*/
	char  *regular_expression1 = {
		"\\<(0*[0-9]{1,2}y)\\s*(0*[0-9]n|1[01]n)\\s*(0*[1-2]?[0-9]d|30d)\\s*"
		"(0*1?[0-9]h|2[0-3]h)\\s*(0*[1-5]?[0-9]m)\\s*(0*[1-5]?[0-9]s)\\>"
	};

	/*
	the second regular expression to check if the string is a valid time spec.
	Each n should be outside of the proper bounds. In order for the string to be
	valid, it should NOT match this pattern.
	[i>=100]y[i>=12]n[i>=31]d[i>=24]h[i>=60]m[i>=60]s. Where i is any unsigned int
	Note: there can be any amount of whitespace between the number letter pairs.
	*/
	char *regular_expression2 = {
		"\\<(99y|[1-9]+[0-9]{2,}y)\\s*([1-9]\\d{2,}n|1[1-9]n|[2-9][0-9]n)\\s*"
		"([3-9][0-9]+d|[1-9]\\d{2,}d)\\s*([1-9][0-9]h|[1-9]\\d{2,}h)\\s*"
		"([3-9][0-9]m|[1-9]\\d{3,}m)\\s*([0-9]+s)\\>"
	};
	/*
	compile and execute the pattern matchinf for the first pattern and save its
	result.
	*/
	ret1 = regcomp(&regex1, regular_expression1, REG_EXTENDED);
	ret1 = regexec(&regex1, str_p, 0, NULL, 0);

	/*
	compile and execute the pattern matching for the second pattern and save its
	result.
	*/
	ret2 = regcomp(&regex2, regular_expression2, REG_EXTENDED);
	ret2 = regexec(&regex2, str_p, 0, NULL, 0);

	// free up the memory used by regex1 and regex2
	regfree(&regex1);
	regfree(&regex2);


	/*
	if the result of the first pattern match resulted in 0 and the second one
	resulted in non-zero, then the string passed the proper tests and is a valid
	time spec, so return 1
	*/
	if (!ret1 && ret2){
		return 1;
	}

	/*
	if the string passed the second test then one of the numbers is out of bounds
	so print error and return 0
	*/
	else if(!ret2){
		fprintf(stderr, "Error in is_legal_time_spec: %s is not a valid time spec, "
		"it is too large\n", str_p);
		return 0;
	}
	/*
	if the string failed the first test then the string is mis-formatted so print
	error and return 0
	*/
	else if(ret1){
	fprintf(stderr, "Error in is_legal_time_spec: %s is not a time spec of the "
	"form\n#y#n#d#h#m#s\n", str_p);
	return 0;
	}
	/*
	this case should not be reached under normal circumstances. Return 0
	*/
	fprintf(stderr, "Error in is_legal_time_spec: fatal error\n");
	return 0;
}


/*
Function checks a string to see if it is a valid amount of seconds. String must
an unsigned int of all digits, and less than 3155760000. Returns 1 (TRUE) if
the string is a valid amount of seconds or prints an error message and returns
0 (FALSE) otherwise.
 */
bool is_legal_seconds(char *sec_p){
	// if the string is less than MAX_SECONDS or it equals 0
	if (atol(sec_p) < MAX_SECONDS || strcmp(sec_p, "0") == 0){
		for ( size_t i=0; i < strlen(sec_p); i++){ // check each char to make sure
			if (!isdigit(sec_p[i])){								 // they are all digits
				return 0;
			}
		}
	}
	else { // if atol(sec_p) > MAX_SECONDS
		fprintf(stderr, "Error in is_legal_seconds: %s is not a valid amount of "
		"seconds, it is too large"
		"\n", sec_p);
		return 0;
	}

	return 1;
}


/*
Function takes in a valid number of seconds as a string and converts it to a
num_seconds_t (u_int32_t)
*/
num_seconds_t string_to_seconds(char* str_p){
	assert(is_legal_seconds(str_p));
	// convert the string to an unsigned long and then to a u_int32_t so it is not
	// hardware dependant
	return (num_seconds_t)atol(str_p);
}

/*
Function takes in a string and saves it's information in a time_spec_t struct.
string is known to be a valid time spec. On success function returns a pointer
to the created time_spec_t struct. Otherwise returns NULL if there was a problem
allocating the memory for the struct.
*/
struct time_spec_t *string_to_time_spec(char *str_p){
	assert(is_legal_time_spec(str_p));
	char copy[strlen(str_p) + 1]; // make a copy of str_p as strtok changes it
	strncpy(copy,str_p, strlen(str_p) + 1);
	char *token; // string to hold each token of str_p
	struct time_spec_t *x = NULL;
	// dinamically allocate memory for a time_spec_t struct so it is the proper
	// size
	x = (struct time_spec_t *)malloc(sizeof(struct time_spec_t));
	if(x == NULL){ // if memory alocation fails, print an error
   fprintf( stderr, "Error in string_to_time_spec: problem allocating memory\n"
  );
	 return NULL;
 	}

	/*
	break each number, letter pair in the time spec string into tokens so you can
	extract each of their values. Using yndhms as the delimiter for each token
	when the time spec has no whitespace between each field and a \s character
	for when there is whitespace in the string. strtok takes consecutive
	delimiter characters as one.
	*/
	token = strtok(copy, "yndhms ");
	x->year = atol(token);

	token = strtok(NULL, "yndhms  ");
	x->month = atol(token);

	token = strtok(NULL, "yndhms  ");
	x->day = atol(token);

	token = strtok(NULL, "yndhms  ");
	x->hour = atol(token);

	token = strtok(NULL, "yndhms  ");
	x->minute = atol(token);

	token = strtok(NULL, "yndhms  ");
	x->second = atol(token);
	// return a pointer to the new struct with the values entered into each field
	return x;
}

/*
Function takes in a num_seconds_t of seconds and converts it into a number of
years, months, days, hours, minutes and seconds. Then stores that information in
a time_spec_t struct. Function returns a pointer to the created struct on
success or returns NULL if there was a problem allocating the memory for the
struct or if the number is too large.
*/
struct time_spec_t *seconds_to_time_spec(num_seconds_t total_seconds){
	struct time_spec_t *converted = NULL;
	if(total_seconds >= MAX_SECONDS){
		fprintf(stderr, "amount of seconds is too large\n");
		return NULL;
	}
	// create a struct to hold all of the time values after the coversions have
	// been done
	converted = (struct time_spec_t *)malloc(sizeof(struct time_spec_t));
	if(converted == NULL){
		// print an error message if the memory couldn't be allocated and return
		// NULL
	 fprintf( stderr, "problem allocating memory\n" );
	 return NULL;
 }

	/*
	When a decimal number is assigned to an int variable, the decimal is chopped
	off. So for each time field find the number with some decimal remainder,
	save as many whole numbers as you can to the first field, then move on to the
	next field by doing the calculation again, subtracting the whole numbers times
	the number of seconds in the greater time field you already saved and dividing
	 the remaining seconds by the seconds in the next smaller time field.
	 Example: 78894000s = 2years 6 months, 78894000 / SECONDS_IN_YEAR = 2.5
	 so years = 2. 78894000 - (2 * SECONDS_IN_YEAR) = 15778800 / SECONDS_IN_MONTH
	 = 6.002.. so months = 6. All time_spec_t fields are defaulted to 0
	*/
	u_int8_t years = total_seconds/SECONDS_IN_YEAR;

	u_int8_t months = (total_seconds - (years * SECONDS_IN_YEAR)) /
	 SECONDS_IN_MONTH;

	u_int8_t days = (total_seconds - (years * SECONDS_IN_YEAR) -
	(months * SECONDS_IN_MONTH)) / SECONDS_IN_DAY ;

	u_int8_t hours = (total_seconds - (years * SECONDS_IN_YEAR) -
	(months * SECONDS_IN_MONTH) - (days * SECONDS_IN_DAY)) / SECONDS_IN_HOUR ;

	u_int8_t minutes = (total_seconds - (years * SECONDS_IN_YEAR) -
	(months * SECONDS_IN_MONTH) - (days * SECONDS_IN_DAY) -
	(hours * SECONDS_IN_HOUR)) / SECONDS_IN_MINUTE;

	u_int8_t seconds = total_seconds - (years * SECONDS_IN_YEAR) -
	(months * SECONDS_IN_MONTH) - (days * SECONDS_IN_DAY) -
	(hours * SECONDS_IN_HOUR) - (minutes * SECONDS_IN_MINUTE);


	converted->year = years;
	converted->month = months;
	converted->day = days;
	converted->hour = hours;
	converted->minute = minutes;
	converted->second = seconds;

	// return a pointer to the created struct
	return converted;
}

/*
Function takes the information from each field of a time_spec_t struct and
converts them into the total amount of seconds. Returns the number of seconds
held by the struct.
*/
num_seconds_t time_spec_to_seconds(struct time_spec_t *times){
	/*
	total equals the sum of each field of the struct times the amount of seconds
	in 1 unit (year,month etc.)
	*/
	num_seconds_t total = (times->year * SECONDS_IN_YEAR) +
	(times->month * SECONDS_IN_MONTH) + (times->day * SECONDS_IN_DAY) +
	(times->hour * SECONDS_IN_HOUR) + (times->minute * SECONDS_IN_MINUTE) +
	(times->second);

	return total;
}

/*
Function prints out the fields of a time_spec_t struct in the proper format
*/
void print_time_spec(struct time_spec_t *time_spec){
	// print the fields of the struct as n1y n2n n3d n4h n5m n6s
	printf("%uy %un %ud %uh %um %us\n", time_spec->year, time_spec->month,
	time_spec->day, time_spec->hour, time_spec->minute, time_spec->second);
}


/*
Function takes in a string and a pointer to a num_seconds_t, if the string is an
amount of seconds, its value is saved to *seconds_p an returns 1 (TRUE).
Else if the string is a valid time spec, converts the string to a time_spec_t
struct, then to an amount of seconds and saves the amount of seconds to
*seconds_p and returns 1. Otherwise returns 0 and prints an error message
*/
bool get_time_arg(char *in_str, num_seconds_t *seconds_p){
	if (is_legal_seconds(in_str)){ // if the string is an amount of seconds
		// assign seconds_p to point to the converted in_str
		*seconds_p = (string_to_seconds(in_str));
		return 1; // success
	}
	else if (is_legal_time_spec(in_str)){ // if the string is a time spec
		// create a struct holding the values of the string
		struct time_spec_t *e = string_to_time_spec(in_str);
		// assign seconds_p to point to the converted in_str
		*seconds_p = time_spec_to_seconds(e);
		return 1; // success
	}
		// if the string is neither a time spec or an amount of seconds, print an
		// error
		fprintf(stderr, "Error in get_time_arg: %s is neither a valid time spec, "
		"nor a valid amount of seconds\n", in_str);
		return 0; // failure
};

/*
Command line function to convert a string of an amount of seconds to a time spec
or vice versa, then prints it.
*/
void conv(char *in_str){
	num_seconds_t sec = 0;
	num_seconds_t *seconds = &sec;
	// convert in_str to seconds and save it to sec for seconds to point to
	get_time_arg(in_str,seconds);

	if(is_legal_seconds(in_str)){ // if the string was seconds
		// convert to a time spec struct and print it
		struct time_spec_t *time = seconds_to_time_spec(sec);
		print_time_spec(time);
	}
	else if(is_legal_time_spec(in_str)){ // if the string was a time spec
		// print the value as seconds
		printf("%u\n", sec);
	}
}

/*
Command line function that subtracts smaller_str from larger_str. Both strings
can be a valid time spec and/or a valid amount of seconds. larger_str must
represent a larger amount of time than smaller_str. If smaller_str > larger_str
then an error message is printed as this would cause a negative time value.
Otherwise the successful subtraction is printed as a time spec.
*/
void subt(char *larger_str, char *smaller_str){
	// convert both strings to seconds
	num_seconds_t sec1 = 0;
	num_seconds_t *large = &sec1;
	get_time_arg(larger_str,large);

	num_seconds_t sec2 = 0;
	num_seconds_t *small = &sec2;
	get_time_arg(smaller_str,small);

	if(*large >= *small) {
		// subtract the smaller amount of seconds from the larger amount of seconds
		num_seconds_t output_seconds = *large - *small;
		// create a time spec struct of the result of the subtraction
		struct time_spec_t *output = seconds_to_time_spec(output_seconds);
		// print the result time spec
		print_time_spec(output);
		}
	else if ((sec1 == 0 && strcmp(larger_str,"0") != 0) ||
	 (sec2 == 0 && strcmp(smaller_str,"0") != 0)) {
			fprintf(stderr, "Error in subt: %s or %s is outside of the valid range\n",
			 larger_str,smaller_str);
		}
	else { // a larger value cannot be subtracted from a smaller value
			fprintf(stderr,"%s cannot be subtracted from %s as this would result in "
			"negative time\n", smaller_str, larger_str );
		}
}


/*
Command line function that adds two time values together and prints the
result as a time spec, time1_str and time2_str can be a valid time spec and/or
a valid amount of seconds. The addition cannot be larger than
MAX_SECONDS/99y11n30d10h30m0s. If this happens an error message is printed.
*/
void add(char *time1_str, char *time2_str){
	// convert both arguments to seconds
	num_seconds_t sec1 = 0;
	num_seconds_t *time1 = &sec1;

	num_seconds_t sec2 = 0;
	num_seconds_t *time2 = &sec2;

	get_time_arg(time1_str,time1);
	get_time_arg(time2_str,time2);


	// add them together and create a time_spec_t struct to hold the result
	num_seconds_t output_seconds = *time1 + *time2;
	if (output_seconds >= MAX_SECONDS) {
		fprintf(stderr, "the addition of %s and %s is too large\n", time1_str,
		time2_str );
	}
	else if (get_time_arg(time1_str,time1) == 0){
		fprintf(stderr, "Error in add: %s is not a valid argument\n", time1_str );
	}
	else if (get_time_arg(time2_str,time2) == 0) {
		fprintf(stderr, "Error in add: %s is not a valid argument\n", time2_str );
	}
	else {
		struct time_spec_t *output = seconds_to_time_spec(output_seconds);
		// print the time spec
		print_time_spec(output);
	}
}

/*
Command line function that compares two amounts of time, time1_str and time2_str
can be a valid amount of seconds and/or a valid time spec.
If time1_str > time2_str, "greater" is printed. If time2_str > time1_str, "less"
is printed. If time1_str = time2_str, "equal" is printed. Otherwise "error" is
printed.
*/
void comp(char *time1_str, char *time2_str){
	// convert time1_str and time2_str to seconds
	num_seconds_t sec1 = 0;
	num_seconds_t *time1 = &sec1;
	get_time_arg(time1_str,time1);

	num_seconds_t sec2 = 0;
	num_seconds_t *time2 = &sec2;
	get_time_arg(time2_str,time2);

	if(*time1 > *time2){
		printf("greater\n");
	}
	else if (*time2 > *time1){
		printf("less\n");
	}
	else if (*time1 == *time2){
		printf("equal\n");
	}
	else{ // unlikely to get here
		fprintf(stderr, "error\n");
	}
}

/*
A function that prints out usage information about the time_calc program
as conv, add, subt and comp are the functions that should be used on the command
line
*/
void usage( void ) {
puts( "Usage: time_calc help" );
puts( "time_calc conv [seconds(int), 'time calc. string'] \n" );
puts( "time calc. string has to have the form #y#n#d#h#m#s, where # are numbers")

puts( "time_calc {add,subt,comp} time1 time2 \n" );
puts( "Here, time1 or time2 can also be an integer representing seconds or a string in the format above")
}

/*
Main function that parses the command line arguments and decides which function
to call
*/
int main(int argc, char*argv[]){
	if (argc == 3){
		if(strcmp("conv",argv[1]) == 0){
			conv(argv[2]);
		}
		else {
			usage();
		}
	}
	else if (argc == 4){
		if (strcmp("subt", argv[1]) == 0) {
			subt(argv[2], argv[3]);
		}
		else if(strcmp("add", argv[1]) == 0){
			add(argv[2], argv[3]);
		}
		else if(strcmp("comp", argv[1]) == 0){
			comp(argv[2], argv[3]);
		}
		else{
			usage();
		}
	}
	else {
		usage();
	}

	return 0;
}
