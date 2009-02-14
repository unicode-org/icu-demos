package com.ibm.icu.impl.locale;

import java.lang.ref.Reference;
import java.lang.ref.ReferenceQueue;
import java.lang.ref.WeakReference;
import java.util.concurrent.ConcurrentHashMap;

public class LocaleObjectPool<K,V> {

    private ConcurrentHashMap<K,Value<V>> _map = new ConcurrentHashMap<K,Value<V>>();
    private ReferenceQueue<V> _rq = new ReferenceQueue<V>();

    public LocaleObjectPool() {
    }

    public V get(Object key) {
        expungeStaleEntries();
        Value<V> val = _map.get(key);
        if (val != null) {
            return val.get();
        }
        return null;
    }

    public V put(K key, V value) {
        expungeStaleEntries();
        Value<V> val = _map.putIfAbsent(key, new WeakValue<V>(key, value, _rq));
        if (val != null) {
            return val.get();
        }
        return null;
    }

    public V putPermanent(K key, V value) {
        expungeStaleEntries();
        Value<V> val = _map.putIfAbsent(key, new StrongValue<V>(value));
        if (val != null) {
            return val.get();
        }
        return null;
    }

    private void expungeStaleEntries() {
        Reference<? extends V> val;
        while ((val = _rq.poll()) != null) {
            Object key = ((WeakValue<?>)val).getKey();
            _map.remove(key);
        }
    }

    private static interface Value<V> {
        V get();
    }

    private static class StrongValue<V> implements Value<V> {
        private V _value;
        public StrongValue(V value) {
            _value = value;
        }
        public V get() {
            return _value;
        }
    }

    private static class WeakValue<V> extends WeakReference<V> implements Value<V>{
        private Object _key;

        public WeakValue(Object key, V value, ReferenceQueue<V> rq) {
            super(value, rq);
            _key = key;
        }

        public V get() {
            return super.get();
        }

        public Object getKey() {
            return _key;
        }
    }
}
