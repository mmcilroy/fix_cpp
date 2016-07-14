require 'util'

counts = {}
routes = { 'BATE', 'CHIX', 'TRQX' }
symbol = 'VOD.L'

function print_counts()
    print( '\n*** message counts' )
    for k,v in pairs( counts ) do
        print( k .. '=' .. v )
    end
end

function place_order( sess, side, symbol, route )
    print( ' ' )
    sess:send( 'D', {
        { 11, fix.id() },
        { 54, side },
        { 55, symbol },
        { 128, route },
        { 50, 'SOR-3' },
        { 38, '10' },
        { 40, '2' },
        { 44, '216' } } );
end

function place_order_batch( sess, count )
    for i = 1, count / 2 do
        route = routes[ i % 3 + 1 ]
        place_order( sess, 1, symbol, route )
        place_order( sess, 2, symbol, route )
    end
end

function on_fix_event( sess, msg )
    type = val( msg, 150 )
    if counts[ type ] == nil then
        counts[ type ] = 1
    else
        counts[ type ] = counts[ type ] + 1
    end
end

function on_timer_event( time )
    if time % 30 == 0 and time > 0 then
        print_counts()
    end
    if time % 10 == 0 then
        place_order_batch( producer, 60 )
    end
end

ems = fix.ems( 'tcp://tibems009-u1.eur.nsroot.net:7009,tcp://tibems009-u2.eur.nsroot.net:7009', 'cssor', 'cssor' )
producer = ems:producer( 'EU.AEGISTOCB2.QA.3', 'FIX.4.4', 'CB2', 'QATEST-3' )
ems:consumer( 'EU.CB2TOAEGIS.QA.3' )

