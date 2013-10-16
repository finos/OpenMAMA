#!/usr/bin/python
# -*- coding: utf-8 -*-
import os
from logger import Logger
from SCons.Script import *
from custom_utils import *

# This is currently just used in windows, though should be reused
# for linux. Well the idea at least. It can be called something else
# then imported in the linux class and added there so eventually all
# SConscripts will be the same.
def install_library(env, source, target):
    """Install the correct library for static/dynamic"""
    target += (env['suffix'])

    if 'dynamic' in env['build']:
        dll, pdb, lib, exp = env.SharedLibrary(target, source)
        library = [ dll, pdb, lib, exp ]
        env.Alias('install', env.Install('$bindir', dll ))
        env.Alias('install', env.Install('$bindir', pdb ))

        if env['vsver_maj'] >= 8:
            env.AddPostAction( library, 'mt.exe -nologo -manifest ${TARGET}.manifest -outputresource:$TARGET;2')


    elif 'static' in env['build']:
        lib = env.StaticLibrary(target, source)
        library = [ lib ]

    env.Alias('install', env.Install('$libdir', lib )) #Always install the .lib
    return library

## Builds c# libs with docs if needed
#
def install_dotnet_library(env, target, source):
    DOC_XML = ""
    if env['with_docs'] == True:
        DOC_XML = env.File('{}\\{}.xml'.format(Dir('.'), target)).abspath
        env.Append( CSCFLAGS = ['/doc:%s' %(DOC_XML) ] )

    if 'dynamic-debug' in env['build']:
        env.Append( CSCFLAGS = ['/debug','/define:DEBUG;TRACE'] )
    elif 'dynamic' in env['build']:
        env.Append( CSCFLAGS = ['/optimize','/define:TRACE'] )

    if 'dynamic' in env['build']:
        library = env.CLILibrary( target ,source )
        env.Alias('install',env.Install('$bindir',library ) )

    Clean(library, DOC_XML)
    return library, DOC_XML

## Create the ndocs from the c# dll file and corresponding xml file
#
def build_ndoc(env, lib, doc_xml, doc_dir):
    #TODO Both of these should be set at the env construction stage
    # and should be done via the vs_ver, check with Ian what the correct
    # versions are, this looks right for 8 & 9 according to pbuilder
    env['NDOC'] = "c:\\windowslibs\\NDoc2\\NDocConsole.exe"
    env['NDOC_DOCUMENTER'] = "MSDN-WEB"
    doc_dir = Dir(doc_dir.strip('#'))
    docs = []
    env.Decider('MD5')
    for l in lib:
        docs.append(env.Command( doc_dir, doc_xml,
            ('{ndoc} -documenter={documenter} -HtmlHelpName="MAMA .Net API" -CopyrightText="© 2012 NYSE Technologies" '
            '-OutputDirectory="{outdir}" -referencepath="{refPath}" '
            '-CleanIntermediates=True -HeaderHTML="<div style=\'width:100%{sc}background-color:white{sc}\'>'
            '<img src=\'nyse_technologies.png\' alt=\'NYSE Technologies\' /><br /><h1 style=\'padding-left:5px{sc}\'>'
            'Middleware Agnostic Messaging API (MAMA) .Net API</h1></div>" {dll},{xml} '
            ''.format(ndoc=env['NDOC'], documenter=env['NDOC_DOCUMENTER'], dll=l.abspath, xml=doc_xml,
                        outdir=doc_dir, sc=';', refPath="objdir\\dynamic\\mama\\dotnet" ))))

    Clean(lib, [ doc_dir, docs ])
    return docs


## Generates symlinks if necessary
#
# Takes a list of ( source, target ) tuples and iterates them. If
# the symlink does not already exist then it creates the symlink.
# The filepaths provided are all relative to the top dir and must
# be within the source tree
def symlinkFiles (env, symlinks):
    host = getHostInfo()
    symboliclinks = []
    topdir = Dir('#').abspath

    for t,s in symlinks:
        s = os.path.join( topdir, s )
        t = os.path.join( topdir, t )

        tdir = os.path.dirname( t )
        symboliclinks.append(t)

        if not os.path.exists( tdir ):
            createDir (tdir )

        if not os.path.exists( t ):
            if host['os'] == 'Windows':
                shutil.copyfile(s, t)
            else:
                os.symlink(s, t)

    return symboliclinks
