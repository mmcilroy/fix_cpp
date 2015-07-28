require 'util'

function on_event( sess, msg )
    msgtype = val( msg, 35 )
    if msgtype == 'A' then
        sess:send( 'A', { { 98, '0' }, { 108, '30' } } )
    elseif msgtype == '0' then
        sess:send( '0', {} )
    elseif msgtype == 'D' then
        sess:send( '8', {
            { 11, val( msg, 11 ) },
            { 55, val( msg, 55 ) },
            { 39, 0 },
            { 150, 0 } } )
    end
end

tcp = fix.tcp()
tcp:accept( ':14002' )
