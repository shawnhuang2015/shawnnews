import sys, re, os.path, time, datetime

if __name__ == '__main__':
    pat = '^(([0-9]+\\.){3}[0-9]+).*\\[([^ ]*) .* JSESSIONID=(.*)$'
    in_file = 'CDNaccess200.log'
    sep = ','

    if len(sys.argv) == 3:
        in_file = sys.argv[1]
        pat = sys.argv[2]

    with open(os.path.expanduser(in_file), 'r') as fp:
        print sep.join(['ip', 'session_id', 'time'])
        for l in fp:
            m = re.search(pat, l.strip())
            if m is not None:
                dtime = datetime.datetime.strptime(m.group(3), '%d/%b/%Y:%H:%M:%S')
                stime = str(int(time.mktime(dtime.timetuple()) * 1000))
                print sep.join([m.group(1), m.group(4), stime])
            else:
                pass
