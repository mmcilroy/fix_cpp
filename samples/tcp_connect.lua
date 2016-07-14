require 'util'

function on_event( sess, msg )
end

tcp = fix.tcp()
sess = tcp:connect( '0.0.0.0:14002', 'FIX.4.4', 'S', 'T' )
sess:send( 'A', { { 98, 0 }, { 108, 0 } } )
sess:send( '0', { {} } )
sess:send( 'D', { { 11, '11111' }, { 55, 'VOD.L' } } )

