import pytest
from setup_test import *
from cfg_classes import *

def pytest_addoption(parser):
    parser.addoption('--sim', default='renode',
                     help='Which sim to use (renode, qemu)')
    # TODO: add optimizations
    parser.addoption('--runtime', default='frtos',
                     help='What runtime should the test be compiled for')
    parser.addoption('--test', default='all',
                     help='Which test(s) to run')
    parser.addoption('--policies', default='simple',
                     help='Which policies to use, or simple, full, etc for combined policies')
    parser.addoption('--rule_cache', default='',
                     help='Which rule cache to use (ideal, finite, dmhc). Empty for none.')
    parser.addoption('--rule_cache_size', default=16,
                     help='size of rule cache, if one is used.')
    parser.addoption('--modules', default='osv.hifive.main',
                     help='which module(s) policies should be referenced from')
    parser.addoption('--composite', default='simple',
                     help='What composite policies (simple, full, else none)')
    
@pytest.fixture
def sim(request):
    return request.config.getoption('--sim')

@pytest.fixture
def runtime(request):
    return request.config.getoption('--runtime')

@pytest.fixture
def rule_cache(request):
    return request.config.getoption('--rule_cache')

@pytest.fixture
def rule_cache_size(request):
    return request.config.getoption('--rule_cache_size')

@pytest.fixture
def composite(request):
    return request.config.getoption('--composite')

def pytest_generate_tests(metafunc):

    test_config = 'hifive' # TODO: remove
    positive_tests = configs[test_config].positive_tests
    negative_tests = configs[test_config].negative_tests

    modules = metafunc.config.option.modules.split(",")

    if 'policy' in metafunc.fixturenames:

        # gather passed policies
        policies = metafunc.config.option.policies.split(",")

        # build composites
        if 'simple' in metafunc.config.option.composite:
            policies = composites(modules, policies, True)
        elif 'full' in metafunc.config.option.composite:
            policies = composites(modules, policies, False)

        # give all policies to test
        metafunc.parametrize("policy", policies, scope='session')

    if 'test' in metafunc.fixturenames:
        if 'all' in metafunc.config.option.test:
            all_tests = positive_tests + negative_tests
            metafunc.parametrize("test", all_tests,
                                 ids=list(map(lambda n: n.replace('/','_'), all_tests)),
                                 scope='session')
        else:
            tests = metafunc.config.option.test.split(",")
            metafunc.parametrize("test", tests,
                                 scope='session')

    if 'rc' in metafunc.fixturenames:
        caches = metafunc.config.option.rule_cache.split(",")
        sizes  = metafunc.config.option.rule_cache_size.split(",")

        # combine rule cache options
        rcs = []
        for c in caches:
            for s in sizes:
                rcs.append((c, s))

        # with only 1 option, don't let rule-cache enter test name
        if len(rcs) == 1:
            rcs = [('','')]

        metafunc.parametrize("rc", rcs,
                             ids=list(map(lambda x: x[0]+x[1], rcs)),
                             scope='session')

# generate the permutations of policies to compose
def permutePols(polStrs):
    p = sorted(polStrs)
    # list of number of policies
    ns = list(range(1,len(p)+1))
    # list of combinations for each n
    combs = [list(map(sorted,itertools.combinations(p, n))) for n in ns]
    # flatten list
    return (reduce(operator.concat, combs, []))

# given modules and policies, generate composite policies
def composites(modules, policies, simple):

    # generate all permutations
    r = []
    for o in modules:
        for p in permutePols(policies):
            r.append((p, pName(o,p)))

    # length of policy that has every member policy except none
    full_composite_len = len(policies)
    if "none" in policies:
        full_composite_len -= 1

    if simple: # single policies or full combination
        return [x[1] for x in r if len(x[0]) == 1 or
                (len(x[0]) == full_composite_len and not "none" in x[0])]
    else: # single policies or any combination without none
        return [x[1] for x in r if len(x[0]) == 1 or not "none" in x[1]]
