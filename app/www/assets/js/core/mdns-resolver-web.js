/**
 * mdns-resolver-web.js
 * Web implementation of mDNS resolver (fallback)
 */

export class MdnsResolverWeb {
    async resolveHost(options) {
        const { hostname } = options;
        
        // Web browsers don't support mDNS resolution
        // Just return the hostname as-is
        console.log('[mDNS Web] No native resolution on web, returning hostname');
        
        return {
            hostname: hostname,
            ipAddress: hostname,
            success: true
        };
    }
}
