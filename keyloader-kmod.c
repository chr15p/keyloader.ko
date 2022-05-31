/*  
 *  keyloader.c  a module for loading keys into keyrings
 */
#include <linux/module.h>   /* Needed by all modules */
#include <linux/kernel.h>   /* Needed for KERN_INFO */

#include <linux/key.h>

#include <linux/moduleparam.h>

// Set module version 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("CProcter");

MODULE_VERSION(KMODVER);

static int keyid=0;
static int platformid=0;
module_param(keyid, uint, 0660);
module_param(platformid, uint, 0660);

// TODO
// check if key already exists in .platform
// remove key from user keyring?
// handle add multiple keys


struct key * get_key_from_id(uint id){
    key_ref_t  key_ref;
    struct key *keyring;

    key_ref = lookup_user_key(id, 0, KEY_NEED_VIEW);
    if (IS_ERR(key_ref)) {
        printk("ERROR to lookup_user_key %d\n", IS_ERR(key_ref));
	return 0;
    }	    
    keyring = key_ref_to_ptr(key_ref);

    return keyring;
}


int init_module(void)
{
    int rc;

    struct key* to_keyring;
    struct key *newkey;

    to_keyring = get_key_from_id(platformid);
    newkey = get_key_from_id(keyid);

    rc = key_link(to_keyring, newkey);
    //rc = key_move(newkey, from_keyring, to_keyring, 0);
    if(rc != 0){
	printk("error linking key\n");
    	return rc;
    }

    return 0;

}


void cleanup_module(void)
{
    struct key* to_keyring;
    struct key *newkey;

    to_keyring = get_key_from_id(platformid);
    newkey = get_key_from_id(keyid);

    key_unlink(to_keyring, newkey);

    printk(KERN_INFO "unloading keyloader.\n");
}
