from unittest import TestLoader, TestCase, TextTestRunner

class TestGlobalContext(TestCase):

    def test_compare(self):
    	self.assertEqual(type(context), ScMemoryContext)

def RunTest(test):
    global TestLoader
    testItem = TestLoader().loadTestsFromTestCase(test)
    res = TextTestRunner(verbosity=2).run(testItem)

    if not res.wasSuccessful():
        raise Exception("Unit test failed")

try:
    RunTest(TestGlobalContext)

except Exception as ex:
    raise ex
except:
    import sys
    print ("Unexpected error:", sys.exc_info()[0])