import os,re,posixpath

## Get the Module Versions
#
# Walk the project tree looking for VERSION.scons files. Read in these files
# and parse the versions accordingly, generating versions which can help
# facilitate building later. The dictionary is then passed to the environment
# for easy access

def get_project_versions(dir):
    versions = {}
    for r,d,f in os.walk(dir):
        for file in f:
            if file == "VERSION.scons":
                f = open( posixpath.join(r,file) )
                line = f.read()
                mod,version = line.split()

                ver = {}
                versions[mod] = {}

                major,minor,release = version.split('.')

                ver['major'] = major
                ver['minor'] = minor
                ver['release'] = "%s" % (release)
                ver['releaseString'] = '%s.%s.%s' % (major, minor, ver['release'])
                ver['build'] = 1

                p = re.compile('^(\d+)(\w)$')
                m = p.match(ver['release'])
                if m:
                    alpha = m.group(2)
                    ver['build'] = ord(alpha)
                    ver['winrelease'] = m.group(1)
                else:
                    ver['build'] = '0'
                    p = re.compile('^(\d+)')
                    m = p.match(ver['release'])
                    if m:
                        ver['winrelease'] = m.group(1)
                versions[mod] = ver
    return versions

def write_versions_file( target, source, env ):
    versions = env['versions']
    
    f = open( target[0].abspath, 'w' )
    for p in sorted( versions ):
        f.write( '%s %s\n' % (p, versions[p]['releaseString']) )
    f.close()

