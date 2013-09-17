#include "msglen.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <CUnit/Basic.h>

#include "test.h"

static void *test_bindata = NULL;
static size_t test_bindata_len = 0;

int init_suite1(void)
{
    void *bindata;
    int fd;
    struct stat statbuf;

    if ( (fd = open(CASES_MPAC, O_RDONLY)) < 0 )
        return 1;

    if ( fstat(fd, &statbuf) < 0 )
        return 1;
    test_bindata_len = statbuf.st_size;

    if ( (bindata = mmap(0, test_bindata_len, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED )
        return 1;

    test_bindata = bindata;

    return 0;
}

int clean_suite1(void)
{
    if (test_bindata)
        if (munmap(test_bindata, test_bindata_len))
            return 1;
    return 0;
}

void testSIMPLE(void)
{
    char *error = NULL;

    char buf[] = {'\xa5', 'H', 'e', 'l', 'l', 'o'};
    size_t n = 6;

    CU_ASSERT_EQUAL(msgpackclen_buf_read (buf, n, &error), n);
    CU_ASSERT_PTR_EQUAL(error, NULL);

    if (error)
        free(error);
}

void testCasesMpac(void)
{
    char *error = NULL;

    CU_ASSERT_EQUAL(
            msgpackclen_buf_read(test_bindata, test_bindata_len, &error),
            test_bindata_len);

    CU_ASSERT_PTR_EQUAL(error, NULL);

    if (error)
        free(error);
}

void testCasesMpacPart(void)
{
    char *error = NULL;

    size_t n = test_bindata_len;
    size_t n1 = n / 2;

    CU_ASSERT_EQUAL(
            msgpackclen_buf_read(test_bindata, n1, &error),
            0);

    CU_ASSERT_PTR_EQUAL(error, NULL);

    CU_ASSERT_EQUAL(
            msgpackclen_buf_read(test_bindata, n, &error),
            test_bindata_len);

    CU_ASSERT_PTR_EQUAL(error, NULL);

    if (error)
        free(error);
}

int main()
{
    CU_pSuite pSuite = NULL;

    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* add a suite to the registry */
    pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (
        (NULL == CU_add_test(pSuite, "Simple test", testSIMPLE)) ||
        (NULL == CU_add_test(pSuite, "cases.mpac test", testCasesMpac)) ||
        (NULL == CU_add_test(pSuite, "cases.mpac (part) test", testCasesMpacPart)) ||
        0)
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
