require 'util'

function on_event( sess, msg )
end

ems = fix.ems( 'tcp://tibems008-d1.eur.nsroot.net:7008', 'ibint', 'ibint' )
sess = ems:producer( 'DEV.GMA.FROM.sm87110', 'FIX.4.4', 'SM87110', 'TEST' )
sess:send( 'D', { { 58, 'hi' } } )
