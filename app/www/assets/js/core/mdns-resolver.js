/**
 * mdns-resolver.js
 * mDNS hostname resolver for Android
 * Resolves .local hostnames using Android NSD API
 */

/**
 * Get or register the MdnsResolver plugin
 * @returns {Object} Plugin instance
 */
function getMdnsResolverPlugin() {
    // Check if Capacitor is available
    if (!window.Capacitor || !window.Capacitor.Plugins) {
        console.warn('[mDNS] Capacitor not available');
        return null;
    }
    
    return window.Capacitor.Plugins.MdnsResolver;
}

/**
 * Resolve hostname to IP address
 * @param {string} hostname - Hostname to resolve (e.g., 'raspberrypi.local')
 * @returns {Promise<{hostname: string, ipAddress: string, success: boolean}>}
 */
export async function resolveHostname(hostname) {
    try {
        console.log('[mDNS] Resolving hostname:', hostname);
        
        // Check if running on native platform
        if (window.Capacitor && window.Capacitor.isNativePlatform && window.Capacitor.isNativePlatform()) {
            const plugin = getMdnsResolverPlugin();
            if (plugin) {
                const result = await plugin.resolveHost({ hostname });
                console.log('[mDNS] Resolved:', result);
                return result;
            }
        }
        
        // Web fallback - just return hostname as-is
        console.log('[mDNS] Running on web, no resolution needed');
        return {
            hostname: hostname,
            ipAddress: hostname,
            success: true
        };
    } catch (error) {
        console.error('[mDNS] Resolution failed:', error);
        throw error;
    }
}

/**
 * Cache for resolved hostnames (to avoid repeated lookups)
 */
const hostnameCache = new Map();
const CACHE_TTL = 5 * 60 * 1000; // 5 minutes

/**
 * Resolve hostname with caching
 * @param {string} hostname - Hostname to resolve
 * @returns {Promise<string>} IP address
 */
export async function resolveHostnameWithCache(hostname) {
    // Check cache first
    const cached = hostnameCache.get(hostname);
    if (cached && Date.now() - cached.timestamp < CACHE_TTL) {
        console.log('[mDNS] Using cached IP:', cached.ipAddress);
        return cached.ipAddress;
    }

    try {
        const result = await resolveHostname(hostname);
        
        // Cache the result
        hostnameCache.set(hostname, {
            ipAddress: result.ipAddress,
            timestamp: Date.now()
        });
        
        return result.ipAddress;
    } catch (error) {
        console.error('[mDNS] Failed to resolve, using hostname as-is:', hostname);
        // Fallback: return original hostname
        return hostname;
    }
}

/**
 * Clear hostname cache
 */
export function clearHostnameCache() {
    hostnameCache.clear();
    console.log('[mDNS] Cache cleared');
}
