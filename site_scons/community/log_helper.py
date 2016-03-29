
def user_value( env, name, value ):
    env['Logger'].log( '%(name) -30s: %(value)-30s [OVERRIDE]' % {'name':name,'value':value}, logtype="CONFIG" )

def default_value( env, name, value ):
    env['Logger'].log( '%(name) -30s: %(value)-30s' % {'name':name,'value':value}, logtype="CONFIG" )

def log_config( env, name, value, default ):
    env.LogDefaultValue( name,default ) if value == default else env.LogUserValue( name, value )

def log_api_config( env ):
    env['Logger'].log_blank()
    env['Logger'].log('API Configuration')
    env['Logger'].log_line()

    env.LogDefaultValue( 'Version Info' , env['versions']['mama']['releaseString'] )
    env.LogConfig( 'With Examples', env['with_examples'], True )
    env.LogConfig( 'With UnitTests', env['with_unittest'], False )
    env.LogConfig( 'With Test Tools', env['with_testtools'], False )
    env.LogConfig( 'With Documentation', env['with_docs'], False )
    env['Logger'].log_line()
    env['Logger'].log_blank()

def Log( env, s, fileonly=False, logtype="INFO" ):
    env['Logger'].log( s, fileonly=fileonly, logtype=logtype )

def LogInfo( env, s ):
    env['Logger'].log( s )

def LogWarning( env, s ):
    env['Logger'].log( s, False, "WARNING" )

def LogDebug( env, s ):
    env['Logger'].log( s, False, "DEBUG" )

def LogError( env, s ):
    env['Logger'].log( s, False, "ERROR" )
