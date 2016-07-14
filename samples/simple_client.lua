require 'util'

function on_timer_event( time )
end

function on_fix_event( sess, msg )
end

function place_order( sess, symbol )
    sess:send( 'D', {
        { 528, 'A' },
        { 11, '116530101363' },
        { 38, '100' },
        { 40, '2' },
        { 44, '66.0' },
        { 50, '01' },
        { 54, 1 },
        { 48, symbol },
        { 59, 0 },
        { 60, fix.time() } } );
end

ems = fix.ems( 'tcp://tibems008-d1.eur.nsroot.net:7008', 'ibint', 'ibint' )
producer = ems:producer( 'QA.CB2.GXET.TO.GMAX', 'FIX.4.4', 'GMAX', 'CLIENT' )
ems:consumer( 'QA.CB2.GMAX.TO.GXET' )

for i=1,1 do
    place_order( producer, '22' )
end

