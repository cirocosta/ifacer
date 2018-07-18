def FlagsForFile( filename, **kwargs ):
        return { 'flags': [
            '-I/usr/include',
            '-I/usr/local/include',
            '-std=gnu99',
            '-xc'
        ]}
