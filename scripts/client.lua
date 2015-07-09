
ems = fix.ems( 'tcp://tibems008-d1.eur.nsroot.net:7008', 'ibint', 'ibint' )

ems:consumer( 'DEV.GMA.TO.sm87110' )
session = ems:producer( 'DEV.GMA.FROM.sm87110', 'FIX.4.4', 'SM87110', 'TEST' )

session:send( 'D', { { 55, 'VOD.L' } } )

function on_event( session, request )
    print( request:str() )
end
