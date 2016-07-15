#!/usr/bin/python -u
# encoding: utf-8
import subprocess, argparse, json, sys, re, dateutil.parser, time
from distutils.version import LooseVersion


RANDOM_SEEDS = [ 42, 194, 458, 886, 2356, 986, 224, 1773, 22, 795, ]

PROGRAM = 'runGA'

def parse_args():
    parser = argparse.ArgumentParser(description='CI Hooks Curler')
    args = parser.parse_args()
    return args

def run_command(command, verbose=False):
    if verbose:
        print '========================================\n\n[COMMAND]:  {0}\n\n'.format(command)
    process = subprocess.Popen(command.encode('utf-8'),stdout=subprocess.PIPE,stderr=subprocess.PIPE,shell=True)
    out, err = process.communicate()
    if verbose:
        print '[STDOUT]:\n{0}\n\n'.format(out)
        print '[STDERR]:\n{0}\n\n'.format(err)
        print '[RETCODE]: {0}\n\n========================================\n\n'.format(process.returncode)
    return out, err, process.returncode

def run_algo(algo, GRAPHS):
    for graph, opt, runtime in GRAPHS:
        for seed in RANDOM_SEEDS:
            command     = './{} data/{} {} {} {} &> /dev/null &'.format(PROGRAM, graph, algo, runtime, seed)
            sol_file    = '{}_GA_{}_{}.sol'.format(graph.split('.')[0], runtime, seed)
            trace_file  = '{}_GA_{}_{}.trace'.format(graph.split('.')[0], runtime, seed)
            # print sol_file, trace_file
            run_command(command)
        while int(run_command('ps -ax | grep \'{}\' | wc -l'.format( './{} data/{} {}'.format(PROGRAM, graph, algo) ))[0].split()[0]) > 2:
            print 'Running for {}...'.format(graph)
            time.sleep(30)
        print 'done!'

def run_ga():
    GRAPHS = [
        ['tiny.graph',          4,          20,]
        ['jazz.graph',          158,        1500,],
        ['karate.graph',        14,         1500,],
        ['football.graph',      94,         1500,],
        ['netscience.graph',    899,        4000,],
        ['email.graph',         594,        4000,],
        ['delaunay_n10.graph',  703,        4000,],
        ['power.graph',         2203,       20000,],
        ['star2.graph',         4542,       120000,],
        ['hep-th.graph',        3926,       40800,],
        ['star.graph',          6902,       120000,],
        ['as-22july06.graph',   3303,       160000,],
        ['star2.graph',         4542,       120000,],
        ['star.graph',          6902,       120000,],
    ]
    run_algo('GA', GRAPHS)

def run_ising():
    GRAPHS = [
    #    ['tiny.graph',          4,          3,],
    #    ['karate.graph',        14,         5,],
    #    ['football.graph',      94,         60,],
    #    ['jazz.graph',          158,        600,],
    #    ['email.graph',         594,        1200,],
    #    ['netscience.graph',    899,        4000,],
    #    ['delaunay_n10.graph',  703,        4000,],
    #    ['power.graph',         2203,       6000,],
    #    ['star2.graph',         4542,       12000,],
    #    ['hep-th.graph',        3926,       12000,],
        ['star.graph',          6902,       24000,],
        ['as-22july06.graph',   3303,       24000,],
    ]
    run_algo('ISING', GRAPHS)


def main():
    # run_ga()
    run_ising()

if __name__ == '__main__':
    main()
