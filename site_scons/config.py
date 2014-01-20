import ConfigParser

class ParseIni:
    def __init__(self):
        self.configuration = {}
        config = ConfigParser.ConfigParser()
        try:
            config.readfp(open('site_scons/enterprise/scons.ini'))
        except:
            config.readfp(open('site_scons/community/scons.ini'))

        if len(config.sections()) > 0:
            for section in config.sections():
                self.configuration[section] = dict(config.items(section))

    def getProductModules( self, product ):
        modules = []
        moduleList = self.configuration['PRODUCTS'][product].split()
        for m in moduleList:
            if not m in modules:
                modules.append(m)
        return modules

    def getProducts( self ):
        products = []
        for p in self.configuration['PRODUCTS']:
            products.append(p)
        return products

    def getSearchStrings( self, section ):
        return self.configuration[section]['search_strings'].split(',')

    def getMwArgs( self, mw ):
        return self.configuration['REGRESSION.args']['{0}_args'.format(mw)]

    def getSection( self, section ):
        return self.configuration[section]
