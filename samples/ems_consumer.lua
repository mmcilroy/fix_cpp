require 'util'

function on_fix_event( sess, msg )
end

function on_timer_event( time )
    print( time )
end

ems = fix.ems( 'tcp://tibems009-u1.eur.nsroot.net:7009,tcp://tibems009-u2.eur.nsroot.net:7009', 'cssor', 'cssor' )
ems:consumer( 'EU.CB2TOAEGIS.QA.3' )

