#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <get.h>
#include <put.h>
#include <pthread.h>
#include <stdbool.h>

#define MAX_KEY_BYTES 1024
#define MAX_VALUE_BYTES 4096
#define KEY_START_VALUE = 10;
#define MAX_NUM_REQUESTS 5
static const bool TEST_MULTI_THREAD = false; 

size_t value_len = 0;
static const char *keys[MAX_NUM_REQUESTS] = {
	"10", "11", "12", "13", "14"
};
static const char *values[MAX_NUM_REQUESTS] = {
	"Ten", "Eleven", "Twelve", "Thirteen", "Fourteen"
};

static void *
thread1_func(void *arg)
{
	int i, ret;
	char *td_arg = (char *) arg;
	char key[MAX_KEY_BYTES] = {0};
	char value[MAX_VALUE_BYTES] = {0};

	printf("%s %d td_arg %s\n", __func__, __LINE__, td_arg);
	for(i = 0; i < MAX_NUM_REQUESTS; i++) {
		snprintf(key, sizeof(key), "%s", keys[i]);
		snprintf(value, sizeof(value), "thread1:%s", values[i]);
		ret = put(key, (void*)value, value_len);
		printf("%s %d Put opr: key %s value %s len %d ret %d\n",
                       __func__, __LINE__, key, value, value_len, ret);
	}
	for(i = 0; i < MAX_NUM_REQUESTS; i++) {
		snprintf(key, sizeof(key), "%s", keys[i]);
		ret = get(key, (void*)value, &value_len);
		printf("%s %d Get opr: key %s value %s len %d ret %d\n",
                       __func__, __LINE__, key, value, value_len, ret);
	}
	return "thread1-exiting";
}

static void *
thread2_func(void *arg)
{
	int i, ret;
	char *td_arg = (char *) arg;
	char key[MAX_KEY_BYTES] = {0};
	char value[MAX_VALUE_BYTES] = {0};

	printf("%s %d td_arg %s\n", __func__, __LINE__, td_arg);
	for(i = 0; i < MAX_NUM_REQUESTS; i++) {
		snprintf(key, sizeof(key), "%s", keys[i]);
		ret = get(key, (void*)value, &value_len);
		printf("%s %d Get opr: key %s value %s len %d ret %d\n",
                       __func__, __LINE__, key, value, value_len, ret);
	}
	
	for(i = 0; i < MAX_NUM_REQUESTS; i++) {
		snprintf(key, sizeof(key), "%s", keys[i]);
		snprintf(value, sizeof(value), "thread2:%s", values[i]);
		ret = put(key, (void*)value, value_len);
		printf("%s %d Put opr: key %s value %s len %d ret %d\n",
                       __func__, __LINE__, key, value, value_len, ret);
	}
	return "thread2-exiting";
}

void
test_multi_thread_operations()
{
	int ret = 0;
	void *res;	
	pthread_t tid1, tid2;
	pthread_attr_t attr1, attr2;

	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);
		
	ret = pthread_create(&tid2, &attr2, &thread2_func, "thread2-arg");
	printf("%s %d pthread_create returned %d, tid %d\n", __func__, __LINE__, ret, tid2);
	ret = pthread_create(&tid1, &attr1, &thread1_func, "thread1-arg");
	printf("%s %d pthread_create returned %d, tid %d\n", __func__, __LINE__, ret, tid1);

	pthread_join(tid1, &res);
	printf("thread1 returned %s\n", res);
	pthread_join(tid2, &res);
	printf("thread2 returned %s\n", res);

	pthread_attr_destroy(&attr1);
	pthread_attr_destroy(&attr2);
}

void
test_single_thread_operations()
{
	int operation;
	printf("Enter operation to be performed (1-Get, 2-Put): \n");
	scanf("%d", &operation);
	char key[MAX_KEY_BYTES] = {0};
	char value[MAX_VALUE_BYTES] = {0};
	fgetc(stdin);
	switch(operation)
	{
		case 1:
		{
			printf("Enter key of object to be obtained: ");
			if (fgets(key, MAX_KEY_BYTES, stdin) == NULL) {
				printf("Key exceeds maximum size\n");
				break;
			}
			if ((strlen(key) > 0) && (key[strlen (key) - 1] == '\n'))
        				key[strlen (key) - 1] = '\0';
			int ret = get(key, (void*)value, &value_len);
			if (ret == 0) {
				printf("Value corresponding to the key is %s and is of length %d\n", 
					(char*)value, value_len);
			}
			if (ret == -1) {
				printf("Key does not exist in the key-value store\n");
			}
			if (ret == -2) {
				printf("Key value exceeds maximum size\n");
			}
			break;
		}

		case 2:
		{
			printf("Enter key of object to be added/updated: ");
			if (fgets(key, MAX_KEY_BYTES, stdin) == NULL) {
				printf("Key exceeds maximum size\n");
				break;
			}
			if ((strlen(key) > 0) && (key[strlen (key) - 1] == '\n'))
        				key[strlen (key) - 1] = '\0';
			printf("Enter value to be added/updated: ");
			if (fgets(value, MAX_VALUE_BYTES, stdin) == NULL) {
				printf("Input exceeds maximum size\n");
			}
			else {
				if ((strlen(value) > 0) && (value[strlen (value) - 1] == '\n'))
        				value[strlen (value) - 1] = '\0';
				int ret = put(key, (void*)value, value_len);
				if (ret == 0) {
					printf("Put operation completed successfully\n");
				}
				if (ret == -1) {
					printf("Error in put operation\n");
				}
				break;
			}
		}
	
		default:
		{
			printf("Invalid operation\n");
		}
	}
}

int main(int argc, char *argv[])
{
 	if (TEST_MULTI_THREAD) {
		test_multi_thread_operations();
	} else {
		test_single_thread_operations();
	}
	return 0;
}

