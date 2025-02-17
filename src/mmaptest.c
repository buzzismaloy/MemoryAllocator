#include "mmapallocator.h"

int main() {
    printf("=== TEST START ===\n");
	int *arr = (int*)mmalloc(10 * sizeof(int));
    int* arr2 = (int*)mmalloc(30 * sizeof(int));
	if (!arr || !arr2) {
        printf("Memory allocation failed\n");
        return 1;
    }

	printf("Array 1:\n");
    for (int i = 0; i < 10; i++) {
        arr[i] = i * 2;
		printf("%d ", arr[i]);
	}
	printf("\n\nArray 2:\n");
	for (int i = 0; i < 30; ++i) {
		arr2[i] = i + 5;
		printf("%d ", arr2[i]);
	}
	printf("\n\n");

    print_memory_list();

	printf("\nMemory is freed\n");
    mfree(arr);
	mfree(arr2);
    print_memory_list();

    return 0;
}
