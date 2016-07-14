require 'util'

i = 0

function on_event( sess, msg )
    msgtype = val( msg, 35 )
    if msgtype == 'A' then
        sess:send( 'A', {
            { 98, '0' },
            { 108, '30' } } )
        sess:send( 'f', {
            { 5574, 'CF' },
            { 5502, 'C' },
            { 5578, 'GRS242003002' },
            { 15, 'EUR' },
            { 5572, '2028.550000' },
            { 460, '5' },
            { 231, '0.000000' },
            { 5530, '0.020000' },
            { 333, '0.016000' },
            { 332, '0.024000' },
            { 48, 'ALTEC' },
            { 22, '8' },
            { 454, '1' },
            { 455, 'BBG000BTQ424' },
            { 456, 'A' },
            { 167, 'CS' },
            { 202, '0.000000' },
            { 207, 'XATH' },
            { 198, '197' } } )
    elseif msgtype == '0' then
        sess:send( '0', {} )
    elseif msgtype == 'D' then
        if i == 0 then
            -- reject the new order
            sess:send( '8', {
                { 11, val( msg, 11 ) },
                { 39, 8 },
                { 150, 8 } } )
            -- send a resend request
            sess:send( '2', {
                { 7, 1 },
                { 16, 0 } } )
            -- ensure order if flushed from xh cache
            fix.sleep( 15 )
            -- ensure we only do this once
            i = 1
        elseif i == 1 then
            -- ack it
            sess:send( '8', {
                { 11, val( msg, 11 ) },
                { 39, 0 },
                { 150, '0' } } )
            -- fill it
            sess:send( '8', {
                { 11, val( msg, 11 ) },
                { 17, fix.id() },
                { 31, '0.022000' },
                { 32, val( msg, 38 ) },
                { 39, 2 },
                { 150, 'F' } } )
            -- ensure we only do this once
            i = 2
       end
    end
end

tcp = fix.tcp()
tcp:accept( ':5002' )

