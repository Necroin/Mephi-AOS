#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    const char* my_env_var = "MyEnvVar";
    const char* my_env_var_value = "MyEnvVarValue";
    
    if(setenv(my_env_var, my_env_var_value, 1) != 0) {
        printf("Error: can't set environment variable with name: %s", my_env_var);
        return 0;
    }

    char* my_env_var_get_value = getenv(my_env_var);
    if(my_env_var_get_value == NULL) {
        printf("Error: can't get environment variable with name: %s", my_env_var);
        return 0;
    }
    printf("environment variable with name \"%s\" have value \"%s\"", my_env_var, my_env_var_get_value);

    unsetenv(my_env_var);

    return 0;
}