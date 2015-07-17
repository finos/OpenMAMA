import subprocess,sys,os
from time import gmtime, strftime,sleep,time

class Logger:
    def __init__(self, opts):
        self.opts = opts
        self.fd = open( opts['logfile'], 'w' )
        self.starttime = time()
        self.fd.write( "%s   -   SCons Builder Invoked\n" % (strftime("%Y-%m-%d %H:%M:%S", gmtime())) )
        self.fd.write( "-----------------------------------------------------------------\n" )

    def log_output( self, sh, escape, cmd, args, env ):
        """Spawn which echos stdout/stderr from the child."""
        asciienv = {}
        #escaped = []
        for key, value in env.iteritems():
            asciienv[key] = str(value)

        #for a in args:
        #   escaped.append( a.replace( "\"","" ) )

        # NOTE:: saw a case where if we capture stdout in the subprocess
        # commands where a file was created using a redirect '>' would fail
        # as STDOUT has been captured already. Modifying this for now to
        # capture the output as on Windows this was causing the source file
        # to be output on every command.
        p = subprocess.Popen(
            ' '.join( args ),
            env=asciienv,
            shell=True,
            stdout = subprocess.PIPE,
            stderr = subprocess.PIPE)
        (stdout,stderr) = p.communicate()

        if len(stderr) > 0:
            if self.opts['verbose'] == True:
               sys.stderr.write('WARNING:: %s' % (stderr))
            self.fd.write('%s\n' % (stderr))
        # Include stdout logs if there are any. This is especially 
        # helpful in Windows builds where MSVS CL command-line sends
        # useful errors/warnings to stdout instead of stderr.
        if len(stdout) > 0:
            if self.opts['verbose'] == True:
               sys.stdout.write('WARNING::stdout: %s' % (stdout))
            self.fd.write('stdout: %s\n' % (stdout))
        return p.returncode

    def log_command(self, s, target, src, env):
        # Only print to screen if we are verbose
        if self.opts['verbose'] == True:
            sys.stdout.write(os.linesep + unicode(s) + os.linesep)
        self.fd.write('%s\n' % (s))

    def log( self, s, fileonly=False, logtype="INFO" ):
        if fileonly == False:
            sys.stdout.write('%s::   %s\n' % (logtype,s))
        self.fd.write('%s::   %s\n' % (logtype,s))

    def log_blank( self ):
        sys.stdout.write("\n")
        self.fd.write("\n")

    def log_line( self ):
        sys.stdout.write( "-----------------------------------------------------------------\n" )
        self.fd.write( "-----------------------------------------------------------------\n" )

    def __exit__(self):
        self.end = time()
        proctime = self.end - self.start
        self.fd.write( "-----------------------------------------------------------------\n" )
        self.fd.write("   Total Processing Time: %.0f\n" % (proctime) )
