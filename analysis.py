#!/usr/bin/python -u
# encoding: utf-8
import subprocess, argparse, csv, sys, re, glob
import numpy as np
import matplotlib.pyplot as plt

def parse_args():
    parser = argparse.ArgumentParser(description='CI Hooks Curler')
    args = parser.parse_args()
    return args

    if args.scm == 'svn' and not args.revision:
        parser.error("--scm is set to SVN; this requires --revision to be set!.")
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

'''
This function looks for the probability of solved instances for (time_sec, relative_error),
where "solved" means solved within relative_error
'''
def find_probability(list_of_list_of_pairs, min_cover_size, time_sec, relative_error):
    min_cover_size  = float(min_cover_size)
    time_sec        = float(time_sec)
    relative_error  = float(relative_error)

    # list_of_list_of_pairs = [ [(timestamp, cover_size), (timestamp, cover_size)...], [(timestamp, cover_size), (timestamp, cover_size)...] ... ]
    cover_sizes = []

    for list_of_pairs in list_of_list_of_pairs:
        c = list_of_pairs[0][0]
        for (timestamp, cover_size) in list_of_pairs:
            if timestamp > time_sec:
                break
            else:
                c = cover_size
        cover_sizes.append(c)

    all_rel_errors = [ (x - min_cover_size) / min_cover_size for x in cover_sizes ]
    return float(len([ x for x in all_rel_errors if x <= relative_error ])) / float(len(list_of_list_of_pairs))

def compute_ranges(list_of_list_of_pairs, min_cover_size, scales=[0.0, 1.0, 0.0, 1.0]):
    max_relative_error      = (max([ l[0][1]  for l in list_of_list_of_pairs ]) - float(min_cover_size)) / min_cover_size
    min_relative_error      = (max([ l[-1][1] for l in list_of_list_of_pairs ]) - float(min_cover_size)) / min_cover_size
    relative_error_range    = max_relative_error - min_relative_error

    min_time                = max([ l[0][0]  for l in list_of_list_of_pairs ])
    max_time                = max([ l[-1][0] for l in list_of_list_of_pairs ])
    time_range              = max_time - min_time

    rel_err_start           = min_relative_error + float(scales[0])*relative_error_range
    rel_err_end             = min_relative_error + float(scales[1])*relative_error_range
    time_start              = min_time + float(scales[2])*time_range
    time_end                = min_time + float(scales[3])*time_range
    return rel_err_start, rel_err_end, time_start, time_end

def qrtd(list_of_list_of_pairs, min_cover_size, num_plots=7, scales=[0.0, 1.0, 0.0, 1.0]):
    rel_err_start, rel_err_end, time_start, time_end    = compute_ranges(list_of_list_of_pairs, min_cover_size, scales)
    rel_errors_increments                               = np.arange(rel_err_start, rel_err_end, (rel_err_end-rel_err_start)/num_plots)

    '''
        Get data in following format:
            [ [timestamp_1, Psolve1_0.2re, Psolve1_0.4re ...], [timestamp_2, Psolve2_0.2re, Psolve2_0.4re ...] ... ]
    '''
    graph_data = []
    for time_sec in np.arange(time_start, time_end, (time_end-time_start)/100.0):
        graph_data.append( [ time_sec ] +
            [ find_probability(list_of_list_of_pairs, min_cover_size, time_sec, relative_error) for relative_error in rel_errors_increments ]
        )

    return {
        'labels': rel_errors_increments,
        'data'  : graph_data,
    }

def sqd(list_of_list_of_pairs, min_cover_size, num_plots=7, scales=[0.0, 1.0, 0.0, 1.0]):
    rel_err_start, rel_err_end, time_start, time_end    = compute_ranges(list_of_list_of_pairs, min_cover_size, scales)
    time_increments                                     = np.arange(time_start, time_end, (time_end-time_start)/num_plots)

    '''
        Get data in following format:
            [ [rel_err_1, Psolve1_0.1s, Psolve1_0.2s ...], [rel_err_2, Psolve2_0.1s, Psolve2_0.2s ...] ... ]
    '''
    graph_data = []
    for relative_error in np.arange(rel_err_start, rel_err_end, (rel_err_end-rel_err_start)/100.0):
        graph_data.append( [ relative_error ] +
            [ find_probability(list_of_list_of_pairs, min_cover_size, time_sec, relative_error) for time_sec in time_increments ]
        )

    return {
        'labels': time_increments,
        'data'  : np.array(graph_data),
    }


# BOXPLOT

def boxplot(list_of_list_of_pairs, min_cover_size):
    # Get the WORST min-cover-approximation
    worst_cover_size = max([ l[-1][1] for l in list_of_list_of_pairs ])
    running_times = []

    for list_of_pairs in list_of_list_of_pairs:
        c = list_of_pairs[0][1]
        for (timestamp, cover_size) in list_of_pairs:
            if cover_size < worst_cover_size:
                break
            else:
                c = timestamp
        running_times.append(c)

    return {
        'cover_size'        : worst_cover_size,
        'running_times'     : running_times,
    }


def convergence(list_of_list_of_pairs, min_cover_size, scales=[0.0, 1.0, 0.0, 1.0]):
    def average_rel_error_at_time(list_of_list_of_pairs, min_cover_size, time_sec):
        min_cover_size  = float(min_cover_size)
        time_sec        = float(time_sec)

        # list_of_list_of_pairs = [ [(timestamp, cover_size), (timestamp, cover_size)...], [(timestamp, cover_size), (timestamp, cover_size)...] ... ]
        cover_sizes = []
        for list_of_pairs in list_of_list_of_pairs:
            c = list_of_pairs[0][0]
            for (timestamp, cover_size) in list_of_pairs:
                if timestamp > time_sec:
                    break
                else:
                    c = cover_size
            cover_sizes.append(c)

        all_rel_errors = [ (x - min_cover_size) / min_cover_size for x in cover_sizes ]
        return float(sum(all_rel_errors))/len(all_rel_errors)

    _, _, time_start, time_end  = compute_ranges(list_of_list_of_pairs, min_cover_size, scales)

    '''
        Get data in following format:
            [ [timestamp_1, Psolve1_0.2re, Psolve1_0.4re ...], [timestamp_2, Psolve2_0.2re, Psolve2_0.4re ...] ... ]
    '''
    graph_data = []
    for time_sec in np.arange(time_start, time_end, (time_end-time_start)/100.0):
        graph_data.append([ time_sec, average_rel_error_at_time(list_of_list_of_pairs, min_cover_size, time_sec) ])

    return {
        'labels': ['average vc size'],
        'data'  : np.array(graph_data),
    }



# FILE IO

def load_trace_file(filepath):
    with open(filepath, 'rb') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=',')
        return [ (float(row[0]), int(row[1])) for row in spamreader ]

def load_trace_files(filepaths):
    return [ load_trace_file(fp) for fp in filepaths ]

def write_qrtd_sqd_output_to_csv(filepath, data):
    print 'writing to {}'.format(filepath)
    with open(filepath, 'wb') as csvfile:
        spamwriter = csv.writer(csvfile, delimiter=',')
        spamwriter.writerow([''] + list(data['labels']))
        for x in data['data']:
            spamwriter.writerow(x)


# TODO !!!
def qrtd_analysis():
    GRAPHS = [
                                                    ### !!!TWEAK PARAMS HERE!!! ###
        ['ISING',   'power.graph',         2203,    [0.0, 0.065, 0.004, 0.035]],
        ['ISING',   'star2.graph',         4542,    [0.0, 0.07, 0.04, 0.35]],
        ['GA',      'power.graph',         2203,    [0.02, 0.1, 0.06, 0.43]],
        ['GA',      'star2.graph',         4542,    [0.01, 0.16, 0.1, 0.35]],
    ]

    for method, graphfilename, mvc, scales in GRAPHS:
        graphfilename_prefix = graphfilename.split('.')[0]
        filepaths = glob.glob('output/{}/{}_*.trace'.format(method.upper(), graphfilename_prefix))
        dataset = load_trace_files(filepaths)

        qrtd_graphs = qrtd(dataset, mvc, 7, scales)
        write_qrtd_sqd_output_to_csv('QRTD_{}_{}.csv'.format(method.upper(), graphfilename_prefix), qrtd_graphs)


def sqd_analysis():
    GRAPHS = [
        ['ISING',   'power.graph',         2203,    [0.0, 0.07, 0.0, 0.04]],
        ['ISING',   'star2.graph',         4542,    [0.0, 0.07, 0.04, 0.3]],
        ['GA',      'power.graph',         2203,    [0.02, 0.1, 0.04, 0.3]],
        ['GA',      'star2.graph',         4542,    [0.01, 0.2, 0.06, 0.3]],
    ]

    for method, graphfilename, mvc, scales in GRAPHS:
        graphfilename_prefix = graphfilename.split('.')[0]
        filepaths = glob.glob('output/{}/{}_*.trace'.format(method.upper(), graphfilename_prefix))
        print filepaths
        dataset = load_trace_files(filepaths)

        sqd_graphs = sqd(dataset, mvc, 7, scales)
        write_qrtd_sqd_output_to_csv('SQD_{}_{}.csv'.format(method.upper(), graphfilename_prefix), sqd_graphs)

def boxplot_analysis():
    GRAPHS = [
        ['ISING',   'power.graph',         2203],
        ['ISING',   'star2.graph',         4542],
        ['GA',      'power.graph',         2203],
        ['GA',      'star2.graph',         4542],
    ]
    for method, graphfilename, mvc in GRAPHS:
        graphfilename_prefix = graphfilename.split('.')[0]
        filepaths = glob.glob('output/{}/{}_*.trace'.format(method.upper(), graphfilename_prefix))
        dataset = load_trace_files(filepaths)

        data = boxplot(dataset, mvc)
        plt.hold(True)
        plt.boxplot(data['running_times'])
        print filepaths
    plt.show()



def convergence_analysis():
    GRAPHS = [
        ['power.graph',         2203,    [0.0, 1.0, 0.0, 0.005]],
        ['star2.graph',         4542,    [0.0, 1.0, 0.0, 0.03]],
        ['as-22july06.graph',   3303,    [0.0, 1.0, 0.0, 1.0]],
    ]

    for graphfilename, mvc, scales in GRAPHS:
        curves = []
        for method in ['ISING', 'GA']:
            graphfilename_prefix = graphfilename.split('.')[0]
            filepaths = glob.glob('output/{}/{}_*.trace'.format(method.upper(), graphfilename_prefix))
            dataset = load_trace_files(filepaths)

            curve = convergence(dataset, mvc, scales)
            write_qrtd_sqd_output_to_csv('CONVERGENCE_{}_{}.csv'.format(method.upper(), graphfilename_prefix), curve)
            curves.append(curve)

        for curve in curves:
            curve['data'] = curve['data'].transpose()

        # plt.plot(curves[0]['data'][0], curves[0]['data'][1], 'bs', curves[1]['data'][0], curves[1]['data'][1], 'g^')
        fig = plt.figure()
        ax1 = fig.add_subplot(111)
        ax1.scatter(curves[0]['data'][0], curves[0]['data'][1], c='b', marker="s", label='ISING')
        ax1.scatter(curves[1]['data'][0], curves[1]['data'][1], c='r', marker="^", label='GA')
        ax1.relim()
        plt.legend(loc='upper right');

        plt.xlabel('Time (s)')
        plt.ylabel('Relative Error')
        fig.tight_layout()
        plt.show()


def main():
   # print 'print test'
   # sqd_analysis()
   # print 'call here'
   # qrtd_analysis()
    boxplot_analysis()
   # convergence_analysis()

if __name__ == '__main__':
    main()
