require 'util'

function on_event( sess, msg )
  print( 'got... ' .. val( msg, 58 ) )
end

ems = fix.ems( 'tcp://tibems008-d1.eur.nsroot.net:7008', 'ibint', 'ibint' )
ems:consumer( 'DEV.GMA.FROM.sm87110' )
