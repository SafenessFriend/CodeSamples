using System.Collections;
using UnityEngine;
using UnityEngine.AI;
using UnityEngine.UI;

public class Boss : MonoBehaviour {

    public int shotsFired = 0;
	int index;

	public float waitingTime = 3;
    public float health = 1000f;
    public float maxHealth = 1000f;
	public Image healthBar;
	public float wanderRadius = 100f;
	float timer;

    public GameObject projectile;
    public GameObject homingProj;
    public GameObject shockwave;
    public GameObject magicStorm;
    public GameObject chargeUp;
    public GameObject death;
    GameObject[] players;
	GameObject[] sWaves;

    public Transform projectileSpawn;
    Transform target;

    Rigidbody rb;
    Animator anim;
	NavMeshAgent agent;
    Material bossMat;
    Vector3 wanderPoint;
	
    private BossActionType eCurState = BossActionType.Attack1;

    void Start ()
    {
        agent = GetComponent<NavMeshAgent>();
        anim = GetComponent<Animator>();
        rb = GetComponent<Rigidbody>();
        Physics.IgnoreLayerCollision(12, 12);
    }

    void Update ()
	{


        anim.SetInteger("shotsFired", shotsFired);        

        players = GameObject.FindGameObjectsWithTag("Player");

        float healthRatio = health / maxHealth;
        healthBar.fillAmount = healthRatio;


        timer += Time.deltaTime;

        if (health <= 0)
        {
            Death();
            Destroy(gameObject);
        }

    }

    
    public void HomingProj()
    {    
        Instantiate(homingProj, transform.position + new Vector3(0, 1, 0), transform.rotation);
    }

    
    public void Shockwave()
    {
        Instantiate(shockwave, transform.position + new Vector3(0, 1, 0), transform.rotation);
    }

    
    void Teleport()
    {
        players = GameObject.FindGameObjectsWithTag("Player");
        index = Random.Range(0, players.Length);
        target = players[index].transform;

        Vector3 position = target.position;
        float distance = Random.Range(10.0f, 30.0f);
        Vector3 objPosition = position - target.forward * distance;
        RaycastHit hit;
        Vector3 startHit = objPosition;
        startHit.y += 200;
        Physics.Raycast(startHit, Vector3.down, out hit);
        objPosition.y = hit.point.y + 0.5f;
        transform.position = objPosition;
    }

    
    void PowerUp()
    {
        Instantiate(chargeUp, transform.position + new Vector3(0, 1, 0), transform.rotation);

    }

    
    void Death()
    {
        Instantiate(death, transform.position + new Vector3(0, 1, 0), transform.rotation);

    }

    public void Projectile()
    {

        Instantiate(projectile, transform.position + new Vector3(0, 2, 0), transform.rotation);
    }

    public void Storm()
    {
        players = GameObject.FindGameObjectsWithTag("Player");
        index = Random.Range(0, players.Length);
        target = players[index].transform;

        Instantiate(magicStorm, target.position + new Vector3(0, 20, 0), transform.rotation);

    }

    public enum BossActionType
	{
		PowerUp,
        Charge,
		Moving,
		Teleport,
		Attack1,
        Attack2
	}

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag == "Projectile")
        {
            health -= 10f;
            Destroy(other.gameObject);
        }
    }


    private void OnParticleCollision(GameObject other)
    {
        if (other.tag == "MagicBlast")
        {

            Debug.Log("MAGICBLASTER");
            health -= 1;

        }
        //else 
        if (other.tag == "Sword")
        {
            health -= 10f;
            Destroy(other.gameObject);
            Debug.Log("SWORD HIT");
        }
    }


    void Wander()
    {
        if (Vector3.Distance(transform.position, wanderPoint) < 0.5f)
        {
            wanderPoint = RandomWanderPoint();
            eCurState = BossActionType.PowerUp;
        }
        else
        {
            agent.SetDestination(wanderPoint);
        }
    }

    private Vector3 RandomWanderPoint()
    {
        Vector3 randomPoint = (Random.insideUnitSphere * wanderRadius) + transform.position;
        NavMeshHit navHit;
        NavMesh.SamplePosition(randomPoint, out navHit, wanderRadius, -1);
        return new Vector3(navHit.position.x, transform.position.y, navHit.position.z);
    }

}