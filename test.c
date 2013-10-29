#include "msglen.h"
#include "bswap.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>

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

void testNewSpecStr8(void)
{
    char *error = NULL;

    char buf[] = {'\xd9', (uint8_t)5, 'H', 'e', 'l', 'l', 'o'};
    size_t n = sizeof(buf);

    CU_ASSERT_EQUAL(msgpackclen_buf_read (buf, n, &error), n);
    CU_ASSERT_PTR_EQUAL(error, NULL);

    if (error)
        free(error);
}

void testNewSpecBin(void)
{
    char *error = NULL;

    union {
        uint16_t num;
        char bytes[2];
    } n16;

    n16.num = 5;
    n16.num = bswap16(n16.num);

    union {
        uint32_t num;
        char bytes[4];
    } n32;

    n32.num = 5;
    n32.num = bswap32(n32.num);

    char buf[] = {'\x93', /* 10010011 */
        '\xc4', (uint8_t)5, 'H', 'e', 'l', 'l', 'o',
        '\xc5', n16.bytes[0], n16.bytes[1], 'H', 'e', 'l', 'l', 'o',
        '\xc6', n32.bytes[0], n32.bytes[1], n32.bytes[2], n32.bytes[3],
                                                    'H', 'e', 'l', 'l', 'o',
    };
    size_t n = sizeof(buf);

    CU_ASSERT_EQUAL(msgpackclen_buf_read (buf, n, &error), n);
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
        (NULL == CU_add_test(pSuite, "New spec str8 test", testNewSpecStr8)) ||
        (NULL == CU_add_test(pSuite, "New spec bin test", testNewSpecBin)) ||
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
